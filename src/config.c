#include "sweetwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 512
#define MAX_SECTIONS 20
#define MAX_KEYVALS 100

typedef struct {
    char key[64];
    char value[256];
} KeyValue;

typedef struct {
    char name[64];
    KeyValue values[MAX_KEYVALS];
    int value_count;
} Section;

typedef struct {
    Section sections[MAX_SECTIONS];
    int section_count;
} Config;

static Config *config = NULL;

static void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

void config_load(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Config not found: %s\n", path);
        return;
    }
    
    if (config) {
        free(config);
        config = NULL;
    }
    
    config = calloc(1, sizeof(Config));
    char line[MAX_LINE];
    int section_idx = -1;
    
    while (fgets(line, sizeof(line), fp)) {
        char *p = line;
        while (*p && isspace(*p)) p++;
        if (*p == '#' || *p == '\n' || *p == '\0') continue;
        
        if (strchr(p, '=') != NULL) {
            char *eq = strchr(p, '=');
            *eq = '\0';
            char *key = p;
            char *val = eq + 1;
            trim(key);
            trim(val);
            
            if (section_idx >= 0 && config->sections[section_idx].value_count < MAX_KEYVALS) {
                KeyValue *kv = &config->sections[section_idx].values[
                    config->sections[section_idx].value_count++];
                strcpy(kv->key, key);
                strcpy(kv->value, val);
            }
        } else {
            char *end = strchr(p, ']');
            if (end) *end = '\0';
            if (*p == '[') p++;
            trim(p);
            section_idx = config->section_count;
            strcpy(config->sections[section_idx].name, p);
            config->sections[section_idx].value_count = 0;
            config->section_count++;
        }
    }
    
    fclose(fp);
}

const char* config_get(const char *section, const char *key, const char *default_val) {
    if (!config) return default_val;
    
    for (int i = 0; i < config->section_count; i++) {
        if (strcmp(config->sections[i].name, section) == 0) {
            for (int j = 0; j < config->sections[i].value_count; j++) {
                if (strcmp(config->sections[i].values[j].key, key) == 0) {
                    return config->sections[i].values[j].value;
                }
            }
        }
    }
    return default_val;
}

int config_get_int(const char *section, const char *key, int default_val) {
    const char *val = config_get(section, key, NULL);
    if (!val) return default_val;
    return atoi(val);
}

int config_get_bool(const char *section, const char *key, int default_val) {
    const char *val = config_get(section, key, NULL);
    if (!val) return default_val;
    return (strcmp(val, "true") == 0 || strcmp(val, "yes") == 0 || strcmp(val, "1") == 0);
}

void config_free(void) {
    if (config) {
        free(config);
        config = NULL;
    }
}
