#pragma once
#include <string>
#include <winerror.h>
#include <winuser.h>

namespace MyUtilities {

	inline void CatchErrorResult(const HRESULT hr, std::string str) {

		if (FAILED(hr)) {

			MessageBox(nullptr, str.c_str(), "ÉGÉâÅ[", MB_OK);
			exit(-1);
		}
	}
}