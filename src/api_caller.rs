use crate::config::Config;
use reqwest::Error;
use serde::{Deserialize, Serialize};

#[derive(Debug, Deserialize, Serialize)]
pub struct Response {
    link: String,
    search_info: SearchInfo,
    pub trips: Vec<Trip>,
}

#[derive(Debug, Deserialize, Serialize)]
struct SearchInfo {
    count: u32,
    full_trip_count: u32,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Trip {
    pub link: String,
    pub waypoints: Vec<Waypoint>,
    pub price: Price,
    pub vehicle: Option<Vehicle>,
    pub distance_in_meters: u32,
    pub duration_in_seconds: u32,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Waypoint {
    pub date_time: String,
    pub place: Place,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Place {
    pub city: String,
    pub address: String,
    pub latitude: f32,
    pub longitude: f32,
    pub country_code: String,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Price {
    pub amount: String,
    pub currency: String,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Vehicle {
    pub make: String,
    pub model: String,
}

pub fn request(config: &Config) -> String {
    format!(
        "https://public-api.blablacar.com/api/v3/trips?\
        from_coordinate={}&to_coordinate={}\
        &locale=fr-FR&currency=EUR\
        &start_date_local={}T00:00:00\
        &count={}\
        &key={}",
        config.from,
        config.to,
        config.date.date(),
        50,
        config.api_key
    )
}

pub async fn response_from_file(filename: &str) -> Result<Response, Error> {
    let file = std::fs::File::open(filename).unwrap();
    let reader = std::io::BufReader::new(file);

    Ok(serde_json::from_reader(reader).unwrap())
}

pub async fn response(request_url: &str) -> Result<Response, reqwest::Error> {
    let response = reqwest::get(request_url).await?;
    Ok(response.json().await?)
}
