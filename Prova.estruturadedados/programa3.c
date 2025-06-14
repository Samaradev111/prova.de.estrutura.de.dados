#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define NUMERO_DE_LEITURAS 2000
#define TAMANHO_MAX_STRING 17

time_t gerar_timestamp(int dia, int mes, int ano, int hora, int minuto, int segundo) {
    struct tm data_hora;
    data_hora.tm_year = ano - 1900;
    data_hora.tm_mon = mes - 1;
    data_hora.tm_mday = dia;
    data_hora.tm_hour = hora;
    data_hora.tm_min = minuto;
    data_hora.tm_sec = segundo;
    data_hora.tm_isdst = -1;
    return mktime(&data_hora);
}

char* gerar_string_aleatoria() {
    static char string_aleatoria[TAMANHO_MAX_STRING];
    int tamanho = rand() % (TAMANHO_MAX_STRING - 1) + 1;  // Entre 1 e 16

    for (int i = 0; i < tamanho; i++) {
        string_aleatoria[i] = 'A' + rand() % 26;
    }
    string_aleatoria[tamanho] = '\0';
    return string_aleatoria;
}

void gerar_valor_aleatorio(char *tipo_dado, char *saida) {
    if (strcmp(tipo_dado, "int") == 0) {
        sprintf(saida, "%d", rand() % 1000);
    } else if (strcmp(tipo_dado, "float") == 0) {
        sprintf(saida, "%.2f", (rand() % 100000) / 100.0);
    } else if (strcmp(tipo_dado, "bool") == 0) {
        strcpy(saida, (rand() % 2) ? "true" : "false");
    } else if (strcmp(tipo_dado, "str") == 0) {
        strcpy(saida, gerar_string_aleatoria());
    } else {
        fprintf(stderr, "Tipo de dado inválido: %s\n", tipo_dado);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 15) {
        fprintf(stderr, "Uso: %s <dia_ini> <mes_ini> <ano_ini> <hora_ini> <min_ini> <seg_ini> <dia_fim> <mes_fim> <ano_fim> <hora_fim> <min_fim> <seg_fim> <arquivo_saida> <sensor:tipo> [...]\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    int dia_inicio = atoi(argv[1]), mes_inicio = atoi(argv[2]), ano_inicio = atoi(argv[3]);
    int hora_inicio = atoi(argv[4]), minuto_inicio = atoi(argv[5]), segundo_inicio = atoi(argv[6]);
    int dia_fim = atoi(argv[7]), mes_fim = atoi(argv[8]), ano_fim = atoi(argv[9]);
    int hora_fim = atoi(argv[10]), minuto_fim = atoi(argv[11]), segundo_fim = atoi(argv[12]);

    const char *nome_arquivo_saida = argv[13];

    time_t timestamp_inicio = gerar_timestamp(dia_inicio, mes_inicio, ano_inicio, hora_inicio, minuto_inicio, segundo_inicio);
    time_t timestamp_fim = gerar_timestamp(dia_fim, mes_fim, ano_fim, hora_fim, minuto_fim, segundo_fim);

    if (timestamp_fim <= timestamp_inicio) {
        fprintf(stderr, "Erro: a data/hora final deve ser posterior à inicial.\n");
        return 1;
    }

    FILE *arquivo = fopen(nome_arquivo_saida, "w");
    if (!arquivo) {
        perror("Erro ao criar o arquivo de saída");
        return 1;
    }

    for (int i = 14; i < argc; i++) {
        char *separador = strchr(argv[i], ':');
        if (!separador) {
            fprintf(stderr, "Formato inválido para sensor: %s (esperado: nome:tipo)\n", argv[i]);
            continue;
        }

        *separador = '\0';
        char *id_sensor = argv[i];
        char *tipo_dado = separador + 1;

        for (int j = 0; j < NUMERO_DE_LEITURAS; j++) {
            time_t timestamp_randomico = timestamp_inicio + rand() % (timestamp_fim - timestamp_inicio + 1);
            char valor_gerado[64];
            gerar_valor_aleatorio(tipo_dado, valor_gerado);
            fprintf(arquivo, "%ld %s %s\n", timestamp_randomico, id_sensor, valor_gerado);
        }
    }

    fclose(arquivo);
    printf("Arquivo '%s' gerado com sucesso.\n", nome_arquivo_saida);
    return 0;
}
