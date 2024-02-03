#include "entity.h"
#include "entity_i.h"
#include <stdlib.h>
#include <furi.h>

#define ENTITY_DEBUG(...) FURI_LOG_D("Entity", __VA_ARGS__)

struct Entity {
    Vector position;
    const EntityBehaviour* behaviour;
    void* context;
};

Entity* entity_alloc(const EntityBehaviour* behaviour) {
    Entity* entity = malloc(sizeof(Entity));
    entity->position = VECTOR_ZERO;
    entity->behaviour = behaviour;
    entity->context = NULL;
    if(behaviour && behaviour->context_size > 0) {
        entity->context = malloc(behaviour->context_size);
    }
    ENTITY_DEBUG("Allocated entity at %p", entity);
    return entity;
}

void entity_free(Entity* entity) {
    ENTITY_DEBUG("Freeing entity at %p", entity);
    if(entity->context) {
        free(entity->context);
    }
    free(entity);
}

Vector entity_pos_get(Entity* entity) {
    return entity->position;
}

void entity_pos_set(Entity* entity, Vector position) {
    entity->position = position;
}

void* entity_context_get(Entity* entity) {
    return entity->context;
}

void entity_call_start(Entity* entity) {
    if(entity->behaviour && entity->behaviour->start) {
        entity->behaviour->start(entity->context);
    }
}

void entity_call_stop(Entity* entity) {
    if(entity->behaviour && entity->behaviour->stop) {
        entity->behaviour->stop(entity->context);
    }
}

void entity_call_update(Entity* entity, Director* director) {
    if(entity->behaviour && entity->behaviour->update) {
        entity->behaviour->update(entity, director, entity->context);
    }
}

void entity_call_render(Entity* entity, Director* director, Canvas* canvas) {
    if(entity->behaviour && entity->behaviour->render) {
        entity->behaviour->render(entity, director, canvas, entity->context);
    }
}