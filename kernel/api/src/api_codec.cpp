#include "api_render.h"

namespace ClientAPI
{
	EKernelAPIStatus cmdCodec(CommandSeq seq)
	{
		enum EIdents { eNone };
		IdentsMap<EIdents> idents_map{};
		std::string token = seq.sliceNextToken();

		EIdents cmd = idents_map.hasIdent(token) ?
			idents_map.getIdent(token) : eNone;

		return EKernelAPIStatus::eErr;
	}
}