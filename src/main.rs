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

    let cars = match car::Cars::new(&config.car_db_filename) {
        Ok(cars) => cars,
        Err(e) => {
            println!("Cannot read car database: {}", e);
            std::process::exit(1);
        }
    };

    let request = api_caller::request(config);
    let response = match api_caller::response(&request).await {
        Ok(response) => response,
        Err(e) => {
            println!("{}", e);
            std::process::exit(1);
        }
    };

    for trip in response.trips.iter() {
        let default_vehicle = api_caller::Vehicle {
            make: String::from("N/A"),
            model: String::from("N/A"),
        };

        let vehicle = match trip.vehicle.as_ref() {
            Some(vehicle) => vehicle,
            None => &default_vehicle,
        };

        let make = &vehicle.make;
        let model = &vehicle.model;
        let key = format!("{}{}", make, model);
        let trunk_size = cars.get_trunk_size(&key);
        println!("{} {}: {:?}", make, model, trunk_size);
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
