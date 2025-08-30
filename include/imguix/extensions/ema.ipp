namespace ImGuiX::Extensions {

    inline void Ema::reset(ImGuiID id, float value) {
        ImGui::GetStateStorage()->SetFloat(id, value);
    }

    inline float Ema::update(ImGuiID id, float sample, float alpha) {
        ImGuiStorage* st = ImGui::GetStateStorage();
        float current = st->GetFloat(id, sample);
        current += alpha * (sample - current);
        st->SetFloat(id, current);
        return current;
    }

    inline float Ema::value(ImGuiID id, float default_value) {
        return ImGui::GetStateStorage()->GetFloat(id, default_value);
    }

} // namespace ImGuiX::Extensions
