use crate::api_caller;
use crate::car;
use crate::config;
use crate::geopoint;

use car::Car;
use car::Cars;
use chrono::NaiveDateTime;
use config::Config;
use geopoint::GeoPoint;
use std::fmt;

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

    pub fn duration_cost(&self) -> f32 {
        0.0001 * self.duration_seconds.pow(2) as f32
    }

    pub fn price_cost(&self) -> f32 {
        (0.25 * self.price).exp()
    }

    pub fn offset_cost(&self, config: &Config) -> f32 {
        let src_offset: u32 = GeoPoint::dist_between(&self.from, &config.from);
        let dest_offset: u32 = GeoPoint::dist_between(&self.to, &config.to);

        (src_offset + dest_offset) as f32
    }

    pub fn depart_time_cost(&self, target: &NaiveDateTime) -> f32 {
        let delta_time = self
            .depart_time
            .signed_duration_since(*target)
            .num_seconds() as f32;
        let delta_time = delta_time / 10000.0;

        // normal distribution
        -24.0 * (-2.0 * delta_time.powi(2)).exp() + 24.0
    }

    pub fn trunk_size_cost(&self) -> f32 {
        match self.vehicle.trunk_size() {
            Some(trunk_size) => 1000.0 / (0.001 * trunk_size as f32),
            None => 0.0,
        }
    }

    pub fn trip_cost(&self, config: &Config, maxs: &TripsMaxCosts) -> f32 {
        let mut sum: f32 = 0.0;

        // divide sum by n to calc an average base on data available.
        // indeed, if trunk size is unknown, we don't want to give an
        // advantage nor a penalty
        let mut n: u8 = 0;

        let mut duration_cost = self.duration_cost();
        // normalize with maximum
        // avoid zero-division error by adding 1.0
        duration_cost /= maxs.duration_cost + 1.0;
        sum += duration_cost;
        n += 1;

        let mut price_cost = self.price_cost();
        price_cost /= maxs.price_cost + 1.0;
        sum += price_cost;
        n += 1;

        let mut offset_cost = self.offset_cost(config);
        offset_cost /= maxs.offset_cost + 1.0;
        sum += offset_cost;
        n += 1;

        let mut depart_time_cost = self.depart_time_cost(&config.date);
        depart_time_cost /= maxs.depart_time_cost + 1.0;
        sum += depart_time_cost;
        n += 1;

        let mut trunk_size_cost = self.trunk_size_cost();
        trunk_size_cost /= maxs.trunk_size_cost + 1.0;
        sum += trunk_size_cost;
        if self.vehicle.trunk_size() != None {
            n += 1;
        };

        sum / n as f32
    }

    pub fn fitness(&self, config: &Config, maxs: &TripsMaxCosts) -> f32 {
        1.0 / self.trip_cost(config, maxs)
    }
}

impl fmt::Display for Trip {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{}€\t{}s\t{}\t{} {}\n{}",
            self.price,
            self.duration_seconds,
            self.depart_time.time(),
            self.vehicle.make(),
            self.vehicle.model(),
            self.link
        )
    }
}

pub struct Trips {
    data: Vec<Trip>,
}

impl Trips {
    pub fn new(api_response: &api_caller::Response, cars_db: &Cars) -> Trips {
        let mut data = Vec::<Trip>::new();

        for trip in api_response.trips.iter() {
            let trip = Trip::new(&trip, &cars_db);
            data.push(trip);
        }

        Trips { data }
    }

    pub fn data(&mut self) -> &mut Vec<Trip> {
        &mut self.data
    }
}

pub struct TripsMaxCosts {
    pub duration_cost: f32,
    pub price_cost: f32,
    pub offset_cost: f32,
    pub depart_time_cost: f32,
    pub trunk_size_cost: f32,
}

impl TripsMaxCosts {
    pub fn new(trips: &Trips, config: &Config) -> Self {
        let mut max_duration_cost = 0.0;
        let mut max_price_cost = 0.0;
        let mut max_offset_cost = 0.0;
        let mut max_depart_time_cost = 0.0;
        let mut max_trunk_size_cost = 0.0;

        for trip in trips.data.iter() {
            let duration_cost = trip.duration_cost();
            if duration_cost > max_duration_cost {
                max_duration_cost = duration_cost;
            };

            let price_cost = trip.price_cost();
            if price_cost > max_price_cost {
                max_price_cost = price_cost;
            };

            let offset_cost = trip.offset_cost(config);
            if offset_cost > max_offset_cost {
                max_offset_cost = offset_cost;
            };

            let depart_time_cost = trip.depart_time_cost(&config.date);
            if depart_time_cost > max_depart_time_cost {
                max_depart_time_cost = depart_time_cost;
            };

            let trunk_size_cost = trip.trunk_size_cost();
            if trunk_size_cost > max_trunk_size_cost {
                max_trunk_size_cost = trunk_size_cost;
            };
        }

        TripsMaxCosts {
            duration_cost: max_duration_cost,
            price_cost: max_price_cost,
            offset_cost: max_offset_cost,
            depart_time_cost: max_depart_time_cost,
            trunk_size_cost: max_trunk_size_cost,
        }
    }
}
