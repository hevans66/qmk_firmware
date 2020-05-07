// !!! DO NOT ADD #pragma once !!! //

// Step 1.
// Declare custom effects using the RGB_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
RGB_MATRIX_EFFECT(custom_splash)

// Step 2.
// Define effects inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static HSV CUSTOM_MATH(HSV hsv, uint8_t dist, uint16_t tick) {
    uint16_t effect = tick + dist * 8;
    if (effect > 255) effect = 255;
    effect = 255 - effect;
    if (effect > 2 && dist > 1){
      hsv.h = hsv.h + 127; // hue is rotated to complementary color
      hsv.v = effect; 
    } else {
      hsv.v = rgb_matrix_config.hsv.v;
    }
    return hsv;
}

typedef HSV (*custom_splash_f)(HSV hsv, uint8_t dist, uint16_t tick);

bool effect_runner_custom(uint8_t start, effect_params_t* params, custom_splash_f effect_func) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t count = g_last_hit_tracker.count;
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        HSV hsv = {
           .h = rgb_matrix_config.hsv.h,
           .s = rgb_matrix_config.hsv.s,
           .v = 0,
        };    
        for (uint8_t j = start; j < count; j++) {
            int16_t  dx   = g_led_config.point[i].x - g_last_hit_tracker.x[j];
            int16_t  dy   = g_led_config.point[i].y - g_last_hit_tracker.y[j];
            uint8_t  dist = sqrt16(dx * dx + dy * dy);
            uint16_t tick = scale16by8(g_last_hit_tracker.tick[j], rgb_matrix_config.speed);
            hsv    = effect_func(hsv, dist, tick);
        }
        RGB result_rgb = hsv_to_rgb(hsv);
	rgb_matrix_set_color(i, result_rgb.r, result_rgb.g, result_rgb.b);
    }
    return led_max < DRIVER_LED_TOTAL;
}

static uint8_t some_global_state;
static void custom_splash_init(effect_params_t* params) {
  some_global_state = 1;
}

static bool custom_splash(effect_params_t* params) {
  if (params->init) custom_splash_init(params);
  return effect_runner_custom(0, params, &CUSTOM_MATH);
}


#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
