#pragma once
#ifndef _IMGUIX_FONTS_FONT_MANAGER_VIEW_CRTP_HPP_INCLUDED
#define _IMGUIX_FONTS_FONT_MANAGER_VIEW_CRTP_HPP_INCLUDED

namespace ImGuiX::Fonts {
	
	template<class Impl>
	struct FontManagerViewCRTP {
		
		ImFont* getFont(FontRole r) const noexcept {
			return static_cast<const Impl*>(this)->getFont(r);
		}
		
		const std::string& activeLocale() const noexcept {
			return static_cast<const Impl*>(this)->activeLocale();
		}
		
		const BuildParams& params() const noexcept { 
			return static_cast<const Impl*>(this)->params(); 
		}
	};

};

#endif // _IMGUIX_FONTS_FONT_MANAGER_VIEW_CRTP_HPP_INCLUDED