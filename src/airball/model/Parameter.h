#ifndef AIRBALL_PARAMETER_H
#define AIRBALL_PARAMETER_H

#include <vector>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <boost/format.hpp>
#include "telemetry/ITelemetry.h"
#include "telemetry/TelemetryIds.h"

namespace airball {

class Parameter {
public:
  Parameter(
      uint16_t message_id,
      const std::string& json_key,
      const std::string& display_name)
      : message_id_(message_id),
        json_key_(json_key),
        display_name_(display_name) {}

  void load(const rapidjson::Document &doc) {
    if (doc.HasMember(json_key().c_str())) {
      loadImpl(doc);
    }
  }

  void save(rapidjson::Document &doc) const {
    saveImpl(doc);
  }

  [[nodiscard]] uint16_t message_id() const { return message_id_; }

  [[nodiscard]] virtual std::string display_value() const = 0;

  [[nodiscard]] std::string display_name() const { return display_name_; }

  virtual void increment() = 0;
  virtual void decrement() = 0;

  ITelemetry::Message toMessage() const {
    ITelemetry::Message m {
      .id = message_id_,
      .data = { 0 },
    };
    toMessageImpl(m);
    return m;
  }

  void fromMessage(const ITelemetry::Message& message) {
    if (message_id_ == message.id) {
      fromMessageImpl(message);
    }
  }

protected:
  [[nodiscard]] std::string json_key() const { return json_key_; }

  virtual void loadImpl(const rapidjson::Document &doc) = 0;
  virtual void saveImpl(rapidjson::Document &doc) const = 0;

  virtual void toMessageImpl(ITelemetry::Message& message) const = 0;
  virtual void fromMessageImpl(const ITelemetry::Message& message) = 0;

private:
  const uint16_t message_id_;
  const std::string json_key_;
  const std::string display_name_;
};

template<typename T>
class TypedParameter : public Parameter {
public:
  TypedParameter(uint16_t message_id,
                 const std::string& json_key,
                 const std::string& display_name,
                 const T& initial)
      : Parameter(message_id, json_key, display_name),
        value_(initial) {}

  [[nodiscard]] T get() const { return value_; }

  [[nodiscard]] std::string display_value() const override;

protected:
  void set(const T& value) { value_ = value; }

  void loadImpl(const rapidjson::Document &doc) override;
  void saveImpl(rapidjson::Document &doc) const override;

  void toMessageImpl(ITelemetry::Message& message) const override {
    memcpy(message.data, &value_, sizeof(T));
  }

  void fromMessageImpl(const ITelemetry::Message& message) override {
    memcpy(&value_, message.data, sizeof(T));
  }

private:
  T value_;
};

template<>
void TypedParameter<int16_t>::loadImpl(const rapidjson::Document &doc) {
  set(doc[json_key().c_str()].GetInt());
}

template<>
void TypedParameter<int16_t>::saveImpl(rapidjson::Document &doc) const {
  doc.AddMember(
      rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
      rapidjson::Value(value_).Move(),
      doc.GetAllocator());
}

template<>
std::string TypedParameter<int16_t>::display_value() const {
  return str(boost::format("%d") % get());
}

template<>
void TypedParameter<double>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[json_key().c_str()].GetDouble();
}

template<>
void TypedParameter<double>::saveImpl(rapidjson::Document &doc) const {
  doc.AddMember(
      rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
      rapidjson::Value(value_).Move(),
      doc.GetAllocator());
}

template<>
std::string TypedParameter<double>::display_value() const {
  return str(boost::format("%f") % get());
}

template<>
void TypedParameter<bool>::loadImpl(const rapidjson::Document &doc) {
  value_ = doc[json_key().c_str()].GetBool();
}

template<>
void TypedParameter<bool>::saveImpl(rapidjson::Document &doc) const {
  doc.AddMember(
      rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
      rapidjson::Value(value_).Move(),
      doc.GetAllocator());
}

template<>
std::string TypedParameter<bool>::display_value() const {
  return get() ? "yes" : "no";
}

class StringSelectionParameter : public Parameter {
public:
  StringSelectionParameter(uint16_t message_id,
                           const std::string& json_key,
                           const std::string& display_name,
                           const std::vector<std::string>& options,
                           const uint16_t initial_index)
      : Parameter(message_id, json_key, display_name),
        current_index_(initial_index),
        options_(options) {}

  [[nodiscard]] std::string get() const {
    return options_[current_index_];
  }

  [[nodiscard]] std::string display_value() const override {
    return get();
  };

  void increment() override {
    current_index_ = std::min(
        (uint16_t)(current_index_ + 1),
        (uint16_t)(options_.size() - 1));
  }

  void decrement() override {
    // Caution about using std::max here since decrementing an unsigned
    // will yield a very large positive number.
    if (current_index_ > 0) {
      current_index_--;
    }
  }

protected:
  void loadImpl(const rapidjson::Document &doc) override {
    std::string string_value = doc[json_key().c_str()].GetString();
    auto i = std::find(options_.begin(), options_.end(), string_value);
    if (i != options_.end()) {
      current_index_ = i - options_.begin();
    }
  }

  void saveImpl(rapidjson::Document &doc) const override {
    doc.AddMember(
        rapidjson::Value(json_key().c_str(), doc.GetAllocator()).Move(),
        rapidjson::Value().SetString(options_[current_index_].c_str(), doc.GetAllocator()).Move(),
        doc.GetAllocator());
  }

  void toMessageImpl(ITelemetry::Message& message) const override {
    memcpy(message.data, &current_index_, sizeof(uint16_t));
  }

  void fromMessageImpl(const ITelemetry::Message& message) override {
    memcpy(&current_index_, message.data, sizeof(uint16_t));
  }

private:
  uint16_t current_index_;
  const std::vector<std::string> options_;
};

class BoolParameter : public TypedParameter<bool> {
public:
  BoolParameter(uint16_t message_id,
                const std::string& json_key,
                const std::string& display_name,
                const bool& initial)
      : TypedParameter<bool>(message_id, json_key, display_name, initial) {}

  void increment() override {
    if (!get()) {
      set(true);
    }
  }

  void decrement() override {
    if (get()) {
      set(false);
    }
  }
};

template<typename T>
class NumericParameter : public TypedParameter<T> {
public:
  NumericParameter(uint16_t message_id,
                   const std::string &json_key,
                   const std::string& display_name,
                   const T &initial,
                   const T &min,
                   const T &max,
                   const T &increment,
                   const std::string& format)
      : TypedParameter<T>(message_id, json_key, display_name, initial),
        min_(min),
        max_(max),
        increment_(increment),
        format_(format) {}

  void increment() override {
    TypedParameter<T>::set(clamp(this->get() + increment_));
  }

  void decrement() override {
    TypedParameter<T>::set(clamp(this->get() - increment_));
  }

  [[nodiscard]] std::string display_value() const override {
    return str(boost::format(format_) % this->get());
  }

private:
  T clamp(T value) {
    T v = (value / increment_) * increment_;
    v = std::max(min_, v);
    v = std::min(max_, v);
    return v;
  }

  const T min_;
  const T max_;
  const T increment_;
  const std::string format_;
};

class SpeedParameter : public NumericParameter<double> {
public:
  SpeedParameter(uint16_t message_id,
                 const std::string &json_key,
                 const std::string &display_name,
                 const double &initial,
                 const double &min,
                 const double &max,
                 const double &increment,
                 const std::string& format,
                 const StringSelectionParameter *speed_units)
      : NumericParameter<double>(message_id, json_key, display_name, initial, min, max, increment, format),
        speed_units_(speed_units) {}

  [[nodiscard]] std::string display_value() const override {
    return str(boost::format("%s %s") %
               NumericParameter < double > ::display_value() %
                                           speed_units_->get());
  }

private:
  const StringSelectionParameter* speed_units_;
};

class AngleParameter : public NumericParameter<double> {
public:
  AngleParameter(uint16_t message_id,
                 const std::string &json_key,
                 const std::string &display_name,
                 const double &initial,
                 const double &min,
                 const double &max,
                 const double &increment,
                 const std::string& format)
      : NumericParameter<double>(message_id, json_key, display_name, initial, min, max, increment, format) {}

  [[nodiscard]] std::string display_value() const override {
    return str(boost::format("%s °") %
               NumericParameter < double > ::display_value());
  }
};

} // namespace airball

#endif // AIRBALL_PARAMETER_H