use std::fmt;
use std::str::FromStr;

#[derive(Debug, PartialEq)]
pub enum ParseGeoPointError {
    InvalidSize,
    InvalidLat,
    InvalidLong,
}

#[derive(Debug, PartialEq)]
pub struct GeoPoint {
    lat: f32,
    long: f32,
}

impl GeoPoint {
    pub fn new(lat: f32, long: f32) -> Self {
        Self { lat, long }
    }
}

impl FromStr for GeoPoint {
    type Err = ParseGeoPointError;

    fn from_str(s_lat_long: &str) -> Result<GeoPoint, Self::Err> {
        let s_lat_long: Vec<&str> = s_lat_long.split(',').collect();
        if s_lat_long.len() != 2 {
            return Err(ParseGeoPointError::InvalidSize);
        }

        let lat = match s_lat_long[0].parse::<f32>() {
            Ok(lat) => lat,
            Err(_err) => return Err(ParseGeoPointError::InvalidLat),
        };
        let long = match s_lat_long[1].parse::<f32>() {
            Ok(long) => long,
            Err(_err) => return Err(ParseGeoPointError::InvalidLong),
        };

        Ok(GeoPoint::new(lat, long))
    }
}

impl fmt::Display for GeoPoint {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{},{}", self.lat, self.long)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parse() {
        assert_eq!(
            Ok(GeoPoint {
                lat: 4.2,
                long: 1.337,
            }),
            GeoPoint::from_str("4.20,1.337")
        );
    }

    #[test]
    fn display() {
        let gp = GeoPoint::from_str("47.124,-0.04500").unwrap();
        assert_eq!("47.124,-0.045", format!("{}", gp));
    }
}
