mod api_caller;
mod car;
mod config;
mod geopoint;

extern crate chrono;
extern crate reqwest;
extern crate serde;
extern crate serde_json;
extern crate tokio;

#[tokio::main]
async fn main() -> Result<(), reqwest::Error> {
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

    let request = api_caller::request(config);
    let response = match api_caller::response(&request).await {
        Ok(response) => response,
        Err(e) => {
            println!("{}", e);
            std::process::exit(1);
        }
    };

    for trip in response.trips.iter() {
        println!("{:?}", trip.vehicle);
    }

    Ok(())
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
