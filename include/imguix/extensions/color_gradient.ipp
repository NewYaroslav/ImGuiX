#include <algorithm>

namespace ImGuiX::Extensions {

    inline ImVec4 hsvToRgb(const Hsv& in) {
        double hh = in.h;
        if (hh >= 360.0)
            hh = 0.0;
        hh /= 60.0;
        long i = static_cast<long>(hh);
        double ff = hh - i;
        double p = in.v * (1.0 - in.s);
        double q = in.v * (1.0 - (in.s * ff));
        double t = in.v * (1.0 - (in.s * (1.0 - ff)));
        ImVec4 out;
        out.w = 1.0f;
        switch (i) {
        case 0:
            out.x = in.v;
            out.y = t;
            out.z = p;
            break;
        case 1:
            out.x = q;
            out.y = in.v;
            out.z = p;
            break;
        case 2:
            out.x = p;
            out.y = in.v;
            out.z = t;
            break;
        case 3:
            out.x = p;
            out.y = q;
            out.z = in.v;
            break;
        case 4:
            out.x = t;
            out.y = p;
            out.z = in.v;
            break;
        default:
            out.x = in.v;
            out.y = p;
            out.z = q;
            break;
        }
        return out;
    }

    inline ColorGradient::ColorGradient(std::size_t count) :
        m_offset(count / 2 + 2 * count / 3),
        m_max_item(count) {}

    inline ImVec4 ColorGradient::next() {
        Hsv col;
        col.h = static_cast<double>((m_next_index + m_offset) % m_max_item)
                / static_cast<double>(m_max_item) * 360.0;
        m_next_index += std::max<std::size_t>(1, m_max_item / 12);
        col.s = static_cast<double>((m_next_index * 2) % m_max_item)
                / static_cast<double>(m_max_item);
        col.v = 0.9;
        return hsvToRgb(col);
    }

} // namespace ImGuiX::Extensions

