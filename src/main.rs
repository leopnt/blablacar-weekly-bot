mod car;
mod config;
mod geopoint;

extern crate chrono;

fn main() {
    let config = match config::Config::new(&mut std::env::args()) {
        Ok(config) => config,
        Err(e) => {
            match e {
                config::ConfigError::InvalidArgs => print_usage(),
                config::ConfigError::InvalidGeopoint(e) => {
                    println!("Invalid GeoPoint: {:?}", e)
                }
                config::ConfigError::InvalidDate(e) => {
                    println!("Invalid Date: {:?}", e)
                }
            }

            std::process::exit(1);
        }
    };

    println!("{:?}\n", config);

    let cars = match car::Car::load_cars(&config.car_db_filename) {
        Ok(cars) => cars,
        Err(e) => {
            println!("Cannot read car database: {}", e);
            std::process::exit(1);
        }
    };

    for i in 0..10 {
        println!("{:?}", cars[i]);
    }
    println!("...\n");

    let request_url = format!(
        "https://public-api.blablacar.com/api/v3/trips?\
        from_coordinate={}&to_coordinate={}\
        &locale=fr-FR&currency=EUR\
        &start_date_local={}T00:00:00\
        &count={}\
        &key={}",
        config.from, config.to, config.date, 20, config.api_key
    );

    println!("{}", request_url);
}

fn print_usage() {
    let prog_name = std::env::args().next().unwrap();

    println!(
        "usage: ./{} <car db filename>.csv \
        <from lat>,<from long> <to lat>,<to long> \
        YYYY-mm-dd \
        <api key>",
        prog_name
    );
}
