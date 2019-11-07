#include "api_render.h"

namespace ClientAPI
{
	APIResponse cmdPlayback(CommandSeq seq)
	{
		enum EIdents { eNone };
		IdentsMap<EIdents> idents_map{};
		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
					  idents_map.getIdent(token) : eNone;

		return APIResponse(EKernelAPIStatus::eErr, "Cannot find such command in 'playback' section");
	}
}