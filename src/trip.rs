use crate::api_caller;
use crate::car;
use crate::geopoint;

use car::Car;
use car::Cars;
use chrono::NaiveDateTime;
use geopoint::GeoPoint;

#[derive(Debug)]
pub struct Trip {
    link: String,
    price: f32,
    duration_seconds: u32,
    depart_time: NaiveDateTime,
    from: GeoPoint,
    to: GeoPoint,
    vehicle: Car,
}

impl Trip {
    pub fn new(api_trip: &api_caller::Trip, cars_db: &Cars) -> Trip {
        let link = String::from(&api_trip.link);
        let price = api_trip.price.amount.parse::<f32>().unwrap();
        let duration_seconds = api_trip.duration_in_seconds;

        let waypoints = &api_trip.waypoints;

        let date_time = &waypoints[0].date_time;

        let s = "%Y-%m-%dT%H:%M:%S";
        let depart_time = NaiveDateTime::parse_from_str(&date_time, s).unwrap();

        let from = GeoPoint::new_from_waypoint(&waypoints[0]);
        let to = GeoPoint::new_from_waypoint(&waypoints[1]);

        let default_vehicle = api_caller::Vehicle {
            make: String::from("N/A"),
            model: String::from("N/A"),
        };

        let vehicle = match api_trip.vehicle.as_ref() {
            Some(vehicle) => vehicle,
            None => &default_vehicle,
        };

        let trunk_size = cars_db.trunk_size(&vehicle.make, &vehicle.model);
        let vehicle = Car::new(&vehicle.make, &vehicle.model, trunk_size);

        Trip {
            link,
            price,
            duration_seconds,
            depart_time,
            from,
            to,
            vehicle,
        }
    }
}
