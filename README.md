# BlaBlaCar Weekly Bot

Find the best trip while doing something else

## 🤨 Why

Because I am lazy and I lose some time each week searching for a trip that:

- isn't too far from my home
- leaves in the evening
- has at least a medium trunk to carry my luggage
- doesn't cost too much
- is fast

## ⚙️ How

### 🧠 The decision process

Each trip has a cost: the higher the cost, the lower the fitness. The trip with the best fitness is the best one.

The fitness is calculated thanks to decision curves that are only relevant to me here.

![decision curves](./doc/cost_functions.png)

The _total offset_ can be seen as the total amount of walking that I'll do.
Note that it is unprecise because it calculates the distance as the crow flies (with earth spherical radius approximation) and doesn't take roads into acount. That's why I thought that a linear function was better/smoother for this one.

![travel offset definition](./doc/travel_offset_definition.png)

## 💻 Usage

```bash
./target/debug/blablacar-weekly-bot <car db filename>.csv <from lat>,<from long> <to lat>,<to long> YYYY-mm-ddTHH:MM:SS <api key>
```

## 📢 Ouput

A sorted list of trips by best.
