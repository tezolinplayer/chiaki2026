#pragma once

#include <stdint.h>
#include <stdbool.h>

// --- CORREÇÃO: Importar as definições do controle aqui ---
#include <chiaki/controller.h>
#include <chiaki/ctrl.h>
// ---------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Configurações do Anti-Recoil
typedef struct {
    bool enabled;
    float vertical_strength;
    int fire_button; // Ex: CHIAKI_CONTROLLER_BUTTON_R2
} AntiRecoilConfig;

// Funções principais
void chiaki_antirecoil_init(void);
void chiaki_antirecoil_process(ChiakiControllerState *state);

#ifdef __cplusplus
}
#endif
