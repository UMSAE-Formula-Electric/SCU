# SCU

Sensor Control Unit

[Documentation](https://docs.google.com/document/d/1yh-vWpTT_wejGpQy9ZQ7YVPmWP_CSU35yJfaNo2MAhA/edit?usp=drive_link)

## Thermistor

[**Temperature Sensors Line Guide**](https://drive.google.com/file/d/1dFKre9OKWxstRY-8zY3tSeb5LHlalR9f/view?usp=drive_link)

[**Honeywell ES110 and ES120 Series**](https://drive.google.com/file/d/1ghP8LBqkBAeznuTnMJgH6ZwvHYQ8_Wdi/view?usp=drive_link)

[**STEINHART & HART Equation**](https://drive.google.com/file/d/1xewpkaQVt0oJnFVwyUym_0Ixe8qXAvnR/view?usp=drive_link)

## Shock Pot
Uses ADC1 to read the voltages and convert them to distances.

[**Shock Pot Datasheet**](https://drive.google.com/file/d/1g9wjH6BT5--y21_IYlu2G4MbX3KbiAo5/view?usp=share_link)

## Flowmeter
Uses TIM12 Channel 1 (pin PB14) to capture the pulses from the flowmeter and determine the flowrate of the fluid in m^3/s.

[**Flowmeter Datasheet**](https://drive.google.com/file/d/1eDgrtt3bu5jgN7wbFmfX5pk9RuAkB_hs/view?usp=share_link)

[**Alternate Flowmeter Datasheet**](https://drive.google.com/file/d/1jgxNPx9BC58do8RVcqYU6hQfMw6DpTo8/view?usp=share_link)

[**Flowmeter Setup Example**](https://drive.google.com/file/d/1NjP9FMI18lw6DNG9UNYPWaCROAbHNBPB/view?usp=share_link)

## CLion
How to get SCU working with CLion?
1. Open the SCU.ioc file as a project in CLion.
2. Choose the Board config to be: stm32f4discovery.cfg
3. In the CMakeLists.txt, uncomment these lines:
```cmake
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
```
