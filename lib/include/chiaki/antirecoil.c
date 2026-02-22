#define _CRT_SECURE_NO_WARNINGS // <--- ISTO OBRIGATÓRIO PARA WINDOWS

#include <chiaki/antirecoil.h>
#include <chiaki/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estado interno global
static struct {
    AntiRecoilConfig config;
    bool is_firing;
    uint64_t fire_start_time;
    float current_y_accum;
} g_recoil = {0};

void chiaki_antirecoil_init(void) {
    g_recoil.config.enabled = false;
    g_recoil.config.vertical_strength = 0;
    g_recoil.config.fire_button = CHIAKI_CONTROLLER_BUTTON_R2;
    g_recoil.is_firing = false;
}

// Função auxiliar para ler o arquivo TXT (Compatível com Windows/Linux)
static void update_config_from_file(void) {
    // CAMINHO DO ARQUIVO - Mude se necessário
    const char* filepath = "C:\\chiaki_recoil.txt"; 
    
    FILE *f = fopen(filepath, "r");
    if (f) {
        int val = 0;
        if (fscanf(f, "%d", &val) == 1) {
            if (val > 0) {
                g_recoil.config.enabled = true;
                // Converte o valor do script (ex: 1500) para força
                g_recoil.config.vertical_strength = (float)val; 
            } else {
                g_recoil.config.enabled = false;
            }
        }
        fclose(f);
    }
}

void chiaki_antirecoil_process(ChiakiControllerState *state) {
    static int frame_check = 0;

    // 1. Atualiza configuração do arquivo TXT a cada 60 chamadas (aprox 1 seg)
    if (frame_check++ > 60) {
        update_config_from_file();
        frame_check = 0;
    }

    if (!g_recoil.config.enabled) return;

    // 2. Detecta se está atirando (R2 pressionado forte)
    // R2 é uint8_t (0-255). > 200 é tiro firme.
    bool firing = (state->r2_state > 200);

    if (firing && !g_recoil.is_firing) {
        g_recoil.is_firing = true;
        g_recoil.fire_start_time = chiaki_time_now_monotonic_ms();
    } else if (!firing) {
        g_recoil.is_firing = false;
    }

    // 3. Aplica o Recoil
    if (g_recoil.is_firing) {
        // Cálculo simples e direto
        int32_t recoil_val = (int32_t)g_recoil.config.vertical_strength;
        
        // Adiciona ao input atual do analógico direito Y
        int32_t new_y = state->right_y + recoil_val;

        // Trava de segurança (Clamp) para não estourar o limite do controle PS5
        if (new_y > 32767) new_y = 32767;
        if (new_y < -32768) new_y = -32768;

        state->right_y = (int16_t)new_y;
    }
}
