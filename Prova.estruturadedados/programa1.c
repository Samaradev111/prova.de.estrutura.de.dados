#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SENSORS 100
#define MAX_READINGS 50
#define MAX_SENSOR_ID 70
#define MAX_VALUE_LEN 80

typedef struct {
    time_t timestamp;
    char value[MAX_VALUE_LEN];
} Reading;

typedef struct {
    char sensor_id[MAX_SENSOR_ID];
    Reading readings[MAX_READINGS];
    int reading_count;
} SensorData;

int compare_readings(const void *a, const void *b) {
    Reading *r1 = (Reading *)a;
    Reading *r2 = (Reading *)b;
    return (r1->timestamp - r2->timestamp);
}

int find_or_add_sensor(SensorData sensors[], int *sensor_count, const char *sensor_id) {
    for (int i = 0; i < *sensor_count; i++) {
        if (strcmp(sensors[i].sensor_id, sensor_id) == 0) {
            return i;
        }
    }
    if (*sensor_count >= MAX_SENSORS) {
        fprintf(stderr, "Erro: número máximo de sensores excedido.\n");
        exit(1);
    }
    strcpy(sensors[*sensor_count].sensor_id, sensor_id);
    sensors[*sensor_count].reading_count = 0;
    return (*sensor_count)++;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo_entrada>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    SensorData sensors[MAX_SENSORS];
    int sensor_count = 0;

    char line[256];
    while (fgets(line, sizeof(line), input_file)) {
        time_t timestamp;
        char sensor_id[MAX_SENSOR_ID];
        char value[MAX_VALUE_LEN];

        if (sscanf(line, "%ld %s %s", &timestamp, sensor_id, value) == 3) {
            int index = find_or_add_sensor(sensors, &sensor_count, sensor_id);
            SensorData *sensor = &sensors[index];
            if (sensor->reading_count >= MAX_READINGS) {
                fprintf(stderr, "Erro: número máximo de leituras excedido para o sensor %s\n", sensor_id);
                continue;
            }
            sensor->readings[sensor->reading_count].timestamp = timestamp;
            strcpy(sensor->readings[sensor->reading_count].value, value);
            sensor->reading_count++;
        }
    }

    fclose(input_file);

    for (int i = 0; i < sensor_count; i++) {
        SensorData *s = &sensors[i];
        qsort(s->readings, s->reading_count, sizeof(Reading), compare_readings);

        char output_filename[100];
        snprintf(output_filename, sizeof(output_filename), "%s.txt", s->sensor_id);

        FILE *output_file = fopen(output_filename, "w");
        if (!output_file) {
            perror("Erro ao criar arquivo de saída");
            continue;
        }

        for (int j = 0; j < s->reading_count; j++) {
            fprintf(output_file, "%ld %s %s\n",
                    s->readings[j].timestamp,
                    s->sensor_id,
                    s->readings[j].value);
        }

        fclose(output_file);
    }

    printf("Processamento concluído com sucesso. %d sensores identificados.\n", sensor_count);
    return 0;
}


