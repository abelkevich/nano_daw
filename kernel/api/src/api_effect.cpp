#include "api_render.h"

namespace ClientAPI
{
    json cmdEffect(CommandSeq seq)
	{
		enum EIdents { eNone };
		IdentsMap<EIdents> idents_map{};
		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;

        return json({ {"error", { {"code", c_err_cannot_find_command_code}, {"msg", c_err_cannot_find_command_str}}} });
	}
}