#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_LINHA 326
#define TAM_ID 60
#define TAM_VALOR 80

typedef struct {
    time_t marca_tempo;
    char sensor_id[TAM_ID];
    char valor_lido[TAM_VALOR];
} DadoSensor;

time_t converter_para_timestamp(int dia, int mes, int ano, int hora, int min, int seg) {
    struct tm t;
    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia;
    t.tm_hour = hora;
    t.tm_min = min;
    t.tm_sec = seg;
    t.tm_isdst = -1;
    return mktime(&t);
}

int carregar_dados(const char *arquivo_sensor, DadoSensor **dados, int *total_dados) {
    FILE *arquivo = fopen(arquivo_sensor, "r");
    if (!arquivo) {
        perror("Erro ao abrir arquivo do sensor");
        return 0;
    }

    int capacidade = 2000;
    *dados = malloc(capacidade * sizeof(DadoSensor));
    *total_dados = 0;

    char linha_entrada[TAM_LINHA];
    while (fgets(linha_entrada, sizeof(linha_entrada), arquivo)) {
        DadoSensor dado;
        if (sscanf(linha_entrada, "%ld %s %s", &dado.marca_tempo, dado.sensor_id, dado.valor_lido) == 3) {
            if (*total_dados >= capacidade) {
                capacidade *= 4;
                *dados = realloc(*dados, capacidade * sizeof(DadoSensor));
            }
            (*dados)[(*total_dados)++] = dado;
        }
    }

    fclose(arquivo);
    return 1
}

int busca_binaria_mais_proxima(DadoSensor *dados, int total_dados, time_t alvo_timestamp) {
    int esquerda = 0, direita = total_dados - 1;
    int indice_mais_proximo = 0;
    long menor_diferenca = labs(dados[0].marca_tempo - alvo_timestamp);

    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        long diferenca = labs(dados[meio].marca_tempo - alvo_timestamp);

        if (diferenca < menor_diferenca) {
            menor_diferenca = diferenca;
            indice_mais_proximo = meio;
        }

        if (dados[meio].marca_tempo < alvo_timestamp) {
            esquerda = meio + 1;
        } else if (dados[meio].marca_tempo > alvo_timestamp) {
            direita = meio - 1;
        } else {
            return meio;
        }
    }

    return indice_mais_proximo;
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        printf("Uso: %s <sensor> <dia> <mes> <ano> <hora> <min> <seg>\n", argv[0]);
        return 1;
    }

    const char *nome_sensor = argv[1];
    int dia = atoi(argv[2]);
    int mes = atoi(argv[3]);
    int ano = atoi(argv[4]);
    int hora = atoi(argv[5]);
    int min = atoi(argv[6]);
    int seg = atoi(argv[7]);

    time_t alvo_timestamp = converter_para_timestamp(dia, mes, ano, hora, min, seg);

    char arquivo_sensor[80];
    snprintf(arquivo_sensor, sizeof(arquivo_sensor), "%s.txt", nome_sensor);

    DadoSensor *dados = NULL;
    int total_dados = 0;

    if (!carregar_dados(arquivo_sensor, &dados, &total_dados)) {
        return 1;
    }

    if (total_dados == 0) {
        printf("Nenhuma leitura encontrada para o sensor %s.\n", nome_sensor);
        free(dados);
        return 1;
    }

    int indice = busca_binaria_mais_proxima(dados, total_dados, alvo_timestamp);
    DadoSensor dado = dados[indice];

    printf("Leitura mais próxima:\n");
    printf("Timestamp: %ld\nSensor: %s\nValor: %s\n", dado.marca_tempo, dado.sensor_id, dado.valor_lido);

    free(dados);
    return 0;
}
