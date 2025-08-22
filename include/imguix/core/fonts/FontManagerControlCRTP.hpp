#pragma once
#ifndef _IMGUIX_FONTS_FONT_MANAGER_CONTROL_CRTP_HPP_INCLUDED
#define _IMGUIX_FONTS_FONT_MANAGER_CONTROL_CRTP_HPP_INCLUDED

/// \file FontManagerControlCRTP.hpp
/// \brief CRTP-фасад для безопасного управления шрифтами в рантайме.
/// \note Все методы должны вызываться на GUI-потоке между кадрами.

namespace ImGuiX::Fonts {

	template<class Impl>
	struct FontManagerControlCRTP {
		// --- Мутация, безопасная в рантайме ---

		/// Смена локали (пересоберёт атлас при следующем rebuildIfNeeded()).
		void setLocale(std::string s) {
			static_cast<Impl*>(this)->setLocale(std::move(s));
		}

		/// Настройка размеров Markdown (px @ 96 DPI).
		void setMarkdownSizes(float body, float h1, float h2, float h3) {
			static_cast<Impl*>(this)->setMarkdownSizes(body, h1, h2, h3);
		}

		/// Логический DPI (масштабирование по дисплею).
		void setDpi(float dpi) {
			static_cast<Impl*>(this)->setDpi(dpi);
		}

		/// Глобальный UI scale (доп. множитель).
		void setUiScale(float ui_scale) {
			static_cast<Impl*>(this)->setUiScale(ui_scale);
		}

		/// Пересборка атласа, если помечен dirty.
		/// \warning Вызывать между кадрами.
		BuildResult rebuildIfNeeded() {
			return static_cast<Impl*>(this)->rebuildIfNeeded();
		}

		// --- Удобные геттеры (read-only) ---

		/// Получить шрифт по роли. Может вернуть nullptr.
		[[nodiscard]] ImFont* getFont(FontRole role) const noexcept {
			return static_cast<const Impl*>(this)->getFont(role);
		}

		/// Текущая активная локаль.
		[[nodiscard]] const std::string& activeLocale() const noexcept {
			return static_cast<const Impl*>(this)->activeLocale();
		}

		/// Текущие параметры сборки (dpi/ui_scale/base_dir/use_freetype) — только чтение.
		[[nodiscard]] const BuildParams& params() const noexcept {
			return static_cast<const Impl*>(this)->params();
		}
	};

} // namespace ImGuiX::Fonts

#endif // _IMGUIX_FONTS_FONT_MANAGER_CONTROL_CRTP_HPP_INCLUDED
