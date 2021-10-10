#![no_std]
#![no_main]

use panic_halt as _;


#[arduino_hal::entry]
fn main() -> ! {
    let dp = arduino_hal::Peripherals::take().unwrap();
    let pins = arduino_hal::pins!(dp);


    let mut led = pins.d13.into_output();
    let mut adc = arduino_hal::Adc::new(dp.ADC, Default::default());
    let a0 = pins.a0.into_analog_input(&mut adc);
    let mut d7 = pins.d7.into_output();
    let mut serial = arduino_hal::default_serial!(dp, pins, 57600);
    let mut read_val;
    let base_value = 1024;
    let full_wet_soil = 615;
    let half_wet = full_wet_soil/2;

    loop {
        led.toggle();
        arduino_hal::delay_ms(20);
        read_val = base_value - a0.analog_read(&mut adc);  
        if read_val >= half_wet {
            ufmt::uwriteln!(&mut serial, "Soil is wet! Stopped watering.");
            d7.set_low();
        }
        else {
            ufmt::uwriteln!(&mut serial, "Soil is dry! Started watering.");
            d7.set_high();

        }
    }
}
