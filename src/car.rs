use std::fs;
use std::str::FromStr;

#[derive(Debug, PartialEq)]
pub enum CsvParseCarError {
    LineSize,
    EmptyCell,
    InvalidTrunkSize,
}

#[derive(Debug, PartialEq)]
pub struct Car {
    model: String,
    make: String,
    trunk_size: u16,
}

impl Car {
    pub fn new(model: &str, make: &str, trunk_size: u16) -> Car {
        Car {
            model: String::from(model),
            make: String::from(make),
            trunk_size,
        }
    }

    pub fn load_cars(csv_filename: &str) -> Result<Vec<Car>, String> {
        let content = match fs::read_to_string(csv_filename) {
            Ok(content) => content,
            Err(err) => return Err(format!("{}: {}", csv_filename, err)),
        };
        let mut content = content.chars();

        let mut cars: Vec<Car> = Vec::<Car>::new();
        let mut line = String::new();

        let mut c = content.next();
        while c != None {
            match c {
                Some('\n') => {
                    let csv_line = line.trim(); // remove \r
                    let car = match Car::from_str(&csv_line) {
                        Ok(car) => car,
                        Err(err) => return Err(format!("{}: {:?}", line, err)),
                    };
                    cars.push(car);
                    line.clear();
                }
                _ => line.push(c.unwrap()),
            }

            c = content.next();
        }

        Ok(cars)
    }
}

impl FromStr for Car {
    type Err = CsvParseCarError;

    fn from_str(csv_line: &str) -> Result<Self, Self::Err> {
        let car_data: Vec<&str> = csv_line.split(",").collect();

        match car_data.len() {
            3 => (),
            _ => return Err(CsvParseCarError::LineSize),
        }

        for cell in car_data.iter() {
            if cell.len() == 0 {
                return Err(CsvParseCarError::EmptyCell);
            }
        }

        let model = car_data[0];
        let make = car_data[1];
        let trunk_size = match car_data[2].parse::<u16>() {
            Ok(size) => size,
            Err(_err) => return Err(CsvParseCarError::InvalidTrunkSize),
        };

        Ok(Car::new(model, make, trunk_size))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn parse() {
        assert_eq!(
            Ok(Car {
                model: String::from("Peugeot"),
                make: String::from("206"),
                trunk_size: 1337,
            }),
            Car::from_str("Peugeot,206,1337")
        );
    }

    #[test]
    fn empty_csv_car_line() {
        assert_eq!("Err(LineSize)", format!("{:?}", Car::from_str("")));
    }

    #[test]
    fn two_cell_line() {
        assert_eq!(
            "Err(LineSize)",
            format!("{:?}", Car::from_str("Peugeot,1337"))
        );
    }

    #[test]
    fn empty_cell() {
        assert_eq!(
            "Err(EmptyCell)",
            format!("{:?}", Car::from_str(",206,1337"))
        );
        assert_eq!(
            "Err(EmptyCell)",
            format!("{:?}", Car::from_str("Peugeot,,1337"))
        );
        assert_eq!(
            "Err(EmptyCell)",
            format!("{:?}", Car::from_str("Peugeot,206,"))
        );
    }

    #[test]
    fn wrong_trunk_size() {
        assert_eq!(
            "Err(InvalidTrunkSize)",
            format!("{:?}", Car::from_str("Peugeot,206,N/A"))
        );
    }
}
