#pragma once

#include "kernel_api.h"
#include "kernel.h"
#include "terminal.h"

#include <sstream>
#include <string>
#include <set>
#include <queue>
#include <map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ClientAPI
{
    extern const std::string c_err_invalid_fragment_id_str;
    extern const uint8_t c_err_invalid_fragment_id_code;

    extern const std::string c_err_invalid_audio_id_str;
    extern const uint8_t c_err_invalid_audio_id_code;

    extern const std::string c_err_invalid_track_id_str;
    extern const uint8_t c_err_invalid_track_id_code;

	extern const std::string c_err_invalid_args_number_str;
    extern const uint8_t c_err_invalid_args_number_code;

	extern const std::string c_err_unimplemented_method_str;
    extern const uint8_t c_err_unimplemented_method_code;

	extern const std::string c_err_invalid_session_str;
    extern const uint8_t c_err_invalid_session_code;

	extern const std::string c_err_operation_failed_str;
    extern const uint8_t c_err_operation_failed_code;

	extern const std::string c_err_invalid_arg_value_str;
    extern const uint8_t c_err_invalid_arg_value_code;

	extern const std::string c_err_cannot_find_command_str;
    extern const uint8_t c_err_cannot_find_command_code;

	class CommandSeq
	{
	private:
		std::queue<std::string> tokens;

	public:
		CommandSeq(std::string cmd_line);
		std::string sliceNextToken();
		bool hasNTokens(uint8_t n);
	};

	template<typename ENUM>
	class IdentsMap
	{
	private:
		const std::map<std::string, ENUM> m_idents;

	public:
		IdentsMap(std::initializer_list<std::pair<const std::string, ENUM>> init_list)
			: m_idents(init_list)
		{
		}

		bool hasIdent(std::string token)
		{
			return m_idents.find(token) != m_idents.end();
		}

		ENUM getIdent(std::string token)
		{
			return m_idents.at(token);
		}
	};

	void initAPI();
}