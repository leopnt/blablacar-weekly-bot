#!/usr/bin/python3

"""Script to filter the car db into a "make,model,avg trunksize" db
"""

import csv

with open('car_db.csv') as csv_file:
    cars: list[list] = []

    csv_reader = csv.reader(csv_file, delimiter=';')

    for row in csv_reader:
        make = row[1]
        model = row[2]
        max_trunk_capacity = -1
        if row[27] != '':
            max_trunk_capacity = round(float(row[27].replace(',', '.')))

        cars.append([make, model, max_trunk_capacity])

cars_filtered: list = []

prev_car: list = cars[0]
car_max_trunk_capacity_sum: int = prev_car[2]
car_max_trunk_capacity_avg: int = 0
duplicate_count: int = 1  # start at one, we count prev_car
for car in cars[1:]:  # skip cars[0] since it is in prev_car
    # check trunksize exists
    if prev_car[2] != -1:
        car_max_trunk_capacity_sum += prev_car[2]
        duplicate_count += 1

    # if make and model match
    if prev_car[:2] != car[:2]:
        # avoid zero division error
        if duplicate_count != 0:
            # mean calculation
            car_max_trunk_capacity_avg = car_max_trunk_capacity_sum
            car_max_trunk_capacity_avg /= duplicate_count

            # add the car
            cars_filtered.append([
                prev_car[0],
                prev_car[1].replace('"', '').replace(
                    ',', ''),  # remove " and ,
                round(car_max_trunk_capacity_avg)
            ])

        # reset values for next car
        car_max_trunk_capacity_sum = 0
        car_max_trunk_capacity_avg = 0
        duplicate_count = 0

    # prepare next iteration
    prev_car = car

with open('car_db_filtered.csv', mode='w') as csv_output_file:
    csv_writer = csv.writer(csv_output_file, delimiter=',')
    csv_writer.writerows(cars_filtered)
