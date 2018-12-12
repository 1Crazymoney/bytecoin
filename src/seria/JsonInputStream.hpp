// Copyright (c) 2012-2018, The CryptoNote developers, The Bytecoin developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include "ISeria.hpp"
#include "common/JsonValue.hpp"
#include "common/Nocopy.hpp"
#include "common/exception.hpp"

namespace seria {

class JsonInputStream : public ISeria {};  // Common base für use with dynamic_cast in ser() methods

class JsonInputStreamValue : public JsonInputStream, private common::Nocopy {
public:
	explicit JsonInputStreamValue(const common::JsonValue &value, bool allow_unused_object_keys);

	bool is_input() const override { return true; }

	void begin_object() override;
	bool object_key(common::StringView name, bool optional) override;
	void end_object() override;

	void begin_map(size_t &size) override;
	void next_map_key(std::string &name) override;
	void end_map() override { end_object(); }

	void begin_array(size_t &size, bool fixed_size) override;
	void end_array() override;

	void seria_v(uint8_t &value) override;
	void seria_v(int16_t &value) override;
	void seria_v(uint16_t &value) override;
	void seria_v(int32_t &value) override;
	void seria_v(uint32_t &value) override;
	void seria_v(int64_t &value) override;
	void seria_v(uint64_t &value) override;
	// void seria_v(double &value) override;
	void seria_v(bool &value) override;
	bool seria_v(std::string &value) override;
	bool seria_v(common::BinaryArray &value) override;
	bool binary(void *value, size_t size) override;

private:
	const common::JsonValue &value;
	const bool allow_unused_object_keys;
	const common::JsonValue *object_key_value = nullptr;
	std::vector<const common::JsonValue *> chain;
	std::vector<size_t> idxs;
	std::vector<common::JsonValue::Object::const_iterator> itrs;
	std::vector<std::set<std::string>> remaining_object_keys;

	const common::JsonValue *get_value();
};

template<typename T, typename... Context>
void from_json_value(T &v, const common::JsonValue &js, Context... context) {
	static_assert(!std::is_pointer<T>::value, "Cannot be called with pointer");
	JsonInputStreamValue s(js, false);
	try {
		ser(v, s, context...);
	} catch (const std::exception &) {
		std::throw_with_nested(std::runtime_error(
		    "Error while deserializing json value of type '" + common::demangle(typeid(T).name()) + "'"));
	}
}
}  // namespace seria
