#pragma once

#include "kernel_api.h"
#include "kernel.h"
#include "terminal.h"

#include <sstream>
#include <string>
#include <set>
#include <queue>
#include <map>

namespace ClientAPI
{
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

	EKernelAPIStatus cmdReceiver(std::string user_cmd_line);
	void sendToClient(std::string msg);
	void initAPI();
}