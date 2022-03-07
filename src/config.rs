use crate::geopoint::GeoPoint;
use crate::geopoint::ParseGeoPointError;
use std::str::FromStr;

use chrono::NaiveDateTime;

#[derive(Debug, PartialEq)]
pub enum ConfigError {
    InvalidArgs,
    InvalidGeopoint(ParseGeoPointError),
    InvalidDate(chrono::ParseError),
}

#[derive(Debug, PartialEq)]
pub struct Config {
    pub car_db_filename: String,
    pub from: GeoPoint,
    pub to: GeoPoint,
    pub date: NaiveDateTime,
    pub api_key: String,
}

impl Config {
    pub fn new(args: &mut std::env::Args) -> Result<Self, ConfigError> {
        // get raw args
        let _prog_name = args.next().unwrap();

        let car_db_filename = match args.next() {
            Some(arg) => arg,
            None => return Err(ConfigError::InvalidArgs),
        };

        let from_arg = match args.next() {
            Some(arg) => arg,
            None => return Err(ConfigError::InvalidArgs),
        };

        let to_arg = match args.next() {
            Some(arg) => arg,
            None => return Err(ConfigError::InvalidArgs),
        };

        let date_arg = match args.next() {
            Some(arg) => arg,
            None => return Err(ConfigError::InvalidArgs),
        };

        let api_key = match args.next() {
            Some(arg) => arg,
            None => return Err(ConfigError::InvalidArgs),
        };

        // check args are correct
        let from = match GeoPoint::from_str(&from_arg) {
            Ok(geopoint) => geopoint,
            Err(e) => return Err(ConfigError::InvalidGeopoint(e)),
        };

        let to = match GeoPoint::from_str(&to_arg) {
            Ok(geopoint) => geopoint,
            Err(e) => return Err(ConfigError::InvalidGeopoint(e)),
        };

        let fmt = "%Y-%m-%dT%H:%M:%S";
        let date = match NaiveDateTime::parse_from_str(&date_arg, fmt) {
            Ok(date) => date,
            Err(e) => return Err(ConfigError::InvalidDate(e)),
        };

        Ok(Self {
            car_db_filename,
            from,
            to,
            date,
            api_key,
        })
    }
}
