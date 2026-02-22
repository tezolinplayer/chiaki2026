#include <chiaki/antirecoil.h>
#include <chiaki/time.h>
#include <string.h>
#include <math.h>

// Estado interno
typedef struct {
    AntiRecoilConfig config;
    bool is_firing;
    uint64_t fire_start_time;
    float accumulated_compensation_x;
    float accumulated_compensation_y;
    int shots_fired;
} AntiRecoilState;

static AntiRecoilState g_recoil_state = {0};

void chiaki_antirecoil_init(AntiRecoilConfig *config) {
    if (!config) {
        g_recoil_state.config.enabled = true;
        g_recoil_state.config.vertical_strength = 35.0f;
        g_recoil_state.config.horizontal_strength = 5.0f;
        g_recoil_state.config.delay_ms = 50;
        g_recoil_state.config.adaptive = true;
        g_recoil_state.config.fire_button = CHIAKI_CONTROLLER_BUTTON_R2;
    } else {
        memcpy(&g_recoil_state.config, config, sizeof(AntiRecoilConfig));
    }
    g_recoil_state.is_firing = false;
}

// Função para simplificar o uso externo (apenas força vertical)
void chiaki_antirecoil_update_recoil_value(int value) {
    if (value > 0) {
        g_recoil_state.config.enabled = true;
        // Converte o valor bruto (ex: 1500) para a escala 0-100 aproximada
        g_recoil_state.config.vertical_strength = (float)value / 50.0f; 
    } else {
        g_recoil_state.config.enabled = false;
    }
}

static float smooth_input(float current, float target, float smoothing) {
    return current + (target - current) * smoothing;
}

void chiaki_antirecoil_process(ChiakiControllerState *state) {
    if (!g_recoil_state.config.enabled) return;

    // Detecta gatilho (R2 é analógico, mas tratamos como botão se > 200)
    bool firing_now = (state->r2_state > 200);

    if (firing_now && !g_recoil_state.is_firing) {
        g_recoil_state.is_firing = true;
        g_recoil_state.fire_start_time = chiaki_time_now_monotonic_ms();
        g_recoil_state.shots_fired = 0;
        g_recoil_state.accumulated_compensation_x = 0;
        g_recoil_state.accumulated_compensation_y = 0;
    } else if (!firing_now && g_recoil_state.is_firing) {
        g_recoil_state.is_firing = false;
    }

    if (g_recoil_state.is_firing) {
        uint64_t current_time = chiaki_time_now_monotonic_ms();
        uint64_t fire_duration = current_time - g_recoil_state.fire_start_time;

        if (fire_duration < g_recoil_state.config.delay_ms) return;

        g_recoil_state.shots_fired++;

        // Cálculo Básico
        float vertical_comp = g_recoil_state.config.vertical_strength;
        
        // Simples cálculo para converter em input do analógico (aprox)
        // Multiplicador arbitrário para converter 0-100 em 0-32767
        float target_y = vertical_comp * 80.0f; 

        // Aplica suavização
        g_recoil_state.accumulated_compensation_y = 
            smooth_input(g_recoil_state.accumulated_compensation_y, target_y, 0.2f);

        // Aplica ao estado
        int32_t new_y = state->right_y + (int32_t)g_recoil_state.accumulated_compensation_y;
        
        // Clamp (limites)
        if (new_y > 32767) new_y = 32767;
        if (new_y < -32768) new_y = -32768;
        
        state->right_y = (int16_t)new_y;
    }
}
