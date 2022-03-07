mod api_caller;
mod car;
mod config;
mod geopoint;
mod trip;

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

    let request = api_caller::request(&config);
    let response = match api_caller::response(&request).await {
        Ok(response) => response,
        Err(e) => {
            println!("Cannot get response: {}", e);
            std::process::exit(1);
        }
    };

    // load trunk sizes database
    let cars = match car::Cars::new(&config.car_db_filename) {
        Ok(cars) => cars,
        Err(e) => {
            println!("Cannot read car database: {}", e);
            std::process::exit(1);
        }
    };

    // make trips from response
    let mut trips = trip::Trips::new(&response, &cars);

    // gather maximum costs
    let trips_max_costs = trip::TripsMaxCosts::new(&trips, &config);

    // sort by fitness
    trips.data().sort_by(|a, b| {
        b.fitness(&config, &trips_max_costs)
            .partial_cmp(&a.fitness(&config, &trips_max_costs))
            .unwrap()
    });

    for trip in trips.data().iter() {
        println!(
            "\n{:?}\nfitness: {}",
            trip,
            trip.fitness(&config, &trips_max_costs)
        );
    }

    Ok(())
}

fn print_usage() {
    let prog_name = std::env::args().next().unwrap();

    println!(
        "usage: ./{} <car db filename>.csv \
        <from lat>,<from long> <to lat>,<to long> \
        YYYY-mm-ddTHH:MM:SS \
        <api key>",
        prog_name
    );
}
