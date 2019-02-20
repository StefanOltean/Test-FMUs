#include <math.h>  // for fabs()
#include "config.h"
#include "model.h"

// shorthand to access the variables
#define M(v) (comp->modelData->v)

#define V_MIN (0.1)

void setStartValues(ModelInstance *comp) {
    M(h) =  1;
    M(v) =  0;
    M(g) = -9.81;
    M(e) =  0.7;
}

void calculateValues(ModelInstance *comp) {
    // do nothing
}

Status getReal(ModelInstance* comp, ValueReference vr, double *value) {
    switch (vr) {
        case vr_h:
            *value = M(h);
            return OK;
        case vr_v:
            *value = M(v);
            return OK;
        case vr_g:
            *value = M(g);
            return OK;
        case vr_e:
            *value = M(e);
            return OK;
        case vr_v_min:
            *value = V_MIN;
            return OK;
        default:
            return Error;
    }
}

Status setReal(ModelInstance* comp, ValueReference vr, double value) {
    switch (vr) {
            
        case vr_h:
            M(h) = value;
            return OK;
            
        case vr_v:
            M(v) = value;
            return OK;
            
        case vr_g:
#if FMI_VERSION > 1
            if (comp->type == ModelExchange &&
				comp->state != modelInstantiated &&
				comp->state != modelInitializationMode) {
                logError(comp, "Variable g can only be set after instantiation or in initialization mode.");
                return Error;
            }
#endif
            M(g) = value;
            return OK;
            
        case vr_e:
#if FMI_VERSION > 1
            if (comp->type == ModelExchange &&
				comp->state != modelInstantiated &&
				comp->state != modelInitializationMode &&
				comp->state != modelEventMode) {
                logError(comp, "Variable e can only be set after instantiation, in initialization mode or event mode.");
                return Error;
            }
#endif
            M(e) = value;
            return OK;
            
        default:
            return Error;
    }
}

void eventUpdate(ModelInstance *comp) {

    if (M(h) <= 0) {

        M(h) = 0;
        M(v) = fabs(M(v) * M(e));

        if (M(v) < V_MIN) {
            // stop bouncing
            M(v) = 0;
            M(g) = 0;
        }

        comp->valuesOfContinuousStatesChanged = true;
    }

    comp->nominalsOfContinuousStatesChanged = false;
    comp->terminateSimulation  = false;
    comp->nextEventTimeDefined = false;
}

void getContinuousStates(ModelInstance *comp, double x[], size_t nx) {
    x[0] = M(h);
    x[1] = M(v);
}

void setContinuousStates(ModelInstance *comp, const double x[], size_t nx) {
    M(h) = x[0];
    M(v) = x[1];
}

void getDerivatives(ModelInstance *comp, double dx[], size_t nx) {
    dx[0] = M(v);
    dx[1] = M(g);
}

void getEventIndicators(ModelInstance *comp, double z[], size_t nz) {
    z[0] = (M(h) == 0 && M(v) == 0) ? 1 : M(h);
}
