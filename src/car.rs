use std::collections::HashMap;
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
    make: String,
    model: String,
    trunk_size: Option<u16>,
}

impl Car {
    pub fn new(make: &str, model: &str, trunk_size: Option<u16>) -> Car {
        Car {
            make: String::from(make),
            model: String::from(model),
            trunk_size,
        }
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

        let make = car_data[0].to_uppercase();
        let model = car_data[1].to_uppercase();
        let trunk_size = match car_data[2].parse::<u16>() {
            Ok(size) => size,
            Err(_err) => return Err(CsvParseCarError::InvalidTrunkSize),
        };

        Ok(Car::new(&make, &model, Some(trunk_size)))
    }
}

pub struct Cars {
    data: HashMap<String, u16>, // "MODELMAKE": trunk_size
}

impl Cars {
    pub fn new(csv_filename: &str) -> Result<Self, String> {
        let content = match fs::read_to_string(csv_filename) {
            Ok(content) => content,
            Err(err) => return Err(format!("{}: {}", csv_filename, err)),
        };
        let mut content = content.chars();

        let mut cars = Cars {
            data: HashMap::<String, u16>::new(),
        };

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

                    let key = format!("{}{}", car.make, car.model);
                    cars.data.insert(key, car.trunk_size.unwrap());

                    line.clear();
                }
                _ => line.push(c.unwrap()),
            }

            c = content.next();
        }
        Ok(cars)
    }

    pub fn trunk_size(&self, make: &str, model: &str) -> Option<u16> {
        let key = format!("{}{}", make, model);
        self.data.get(&key).copied()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn parse() {
        assert_eq!(
            Ok(Car {
                make: String::from("PEUGEOT"),
                model: String::from("206"),
                trunk_size: Some(1337),
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
