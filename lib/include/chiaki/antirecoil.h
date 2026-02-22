#pragma once

#include <chiaki/ctrl.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Configurações do Anti-Recoil
typedef struct {
    bool enabled;
    float vertical_strength;      // 0.0 - 100.0
    float horizontal_strength;    // 0.0 - 100.0
    int delay_ms;                 // Delay antes de aplicar (0-500ms)
    bool adaptive;                // Compensação adaptativa
    int fire_button;              // R2 por padrão
} AntiRecoilConfig;

// Funções
void chiaki_antirecoil_init(AntiRecoilConfig *config);
void chiaki_antirecoil_process(ChiakiControllerState *state);
void chiaki_antirecoil_toggle(bool enabled);
void chiaki_antirecoil_set_vertical_strength(float strength);
void chiaki_antirecoil_update_recoil_value(int value); // Função auxiliar para seu txt

#ifdef __cplusplus
}
#endif
