


#pragma once

// Constant max velocity between t1 and t2
// t1 and t2 are constrained to 0 to 1
template<typename VelocityType>
class EaseInOutVelocity {
public:
    EaseInOutVelocity() = default;
    EaseInOutVelocity(float t1, float t2) : t1(t1), t2(t2), velocity(2.0f / (1 - t1 + t2))
    {

    }

    VelocityType CalcDistance(float t) const
    {
        // t = std::clamp(t, 0.f, 0.99f);
        if (0 <= t && t <= t1)
        {
            return (velocity / (2 * t1)) * (t * t);
        }
        else if (t1 < t && t < t2)
        {
            return velocity * (t - (t1 / 2.0f));
        }
        else if (t2 <= t && t <= 1)
        {
            return ((velocity * (t - t2)) / (2 * (1 - t2))) * (2 - t - t2) + velocity * (t2 - (t1 / 2.0f));
        }
        else
        {
            return {};
        }

    }

    VelocityType GetVelocity(float t) const
    {
        // t = std::clamp(t, 0.f, 1.f);
        if (0 <= t && t <= t1)
        {
            return velocity * t / t1;
        }
        else if (t1 < t && t < t2)
        {
            return velocity;
        }
        else if (t2 <= t && t <= 1)
        {
            return velocity * (1 - t) / (1 - t2);
        }
        else
        {
            return {};
        }
    }

    bool isOverTime(float currentTime) const
    {
        return currentTime > 1;
    }

    void ImguiDisplay()
    {
        ImGui::TextColored({1.0f, 0.1, 1.0f, 1.0f}, "Magenta"); ImGui::SameLine();
        ImGui::TextWrapped(" colored balls to visually signify t1 and t2 on the curve.");
        ImGui::Text("Velocity: %f", velocity);
        ImGui::DragFloat("T1", &t1, 0.01f, 0.0f, t2);
        ImGui::DragFloat("T2", &t2, 0.01f, t1, 1.0f);

        velocity = (2.0f / (1 - t1 + t2));
    }

    float GetT1() const
    {
        return t1;
    }

    float GetT2() const
    {
        return t2;
    }


    void SetVars(float t1, float t2)
    {
        this->t1 = t1;
        this->t2 = t2;
        velocity = (2.0f / (1 - t1 + t2));
    }

private:
    float t1 {};
    float t2 {};
    VelocityType velocity {};
};



