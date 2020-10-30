class Area {
    constructor() {
        this.district = 0;
        this.infectionRate = 0;
        this.healthRate = 0;
        this.population = 0;
    }
}

class Reader {
    constructor() {
        this.maxtick = 0;
        this.gameid = 0;
        this.countries_counts = 0;
        this.dimension = [];
        this.factorHistory = [];
        this.gameHistory = [];
        this.infectionHistory = [[]];
        this.healingHistory = [[]];
    }

    loadData (response) {
        const msg = response.split("\n");
        for (let i = 0; i < msg.length; i++) {
            const words = msg[i].split(" ");
            if (words[0] === "START") {
                this.gameid = words[1];
                this.maxtick = words[2];
                this.countries_counts = words[3];
            } else if (words[0] === "FACTORS") {
                let factors = [words[1], words[2], words[3], words[4]];
                this.factorHistory.push(factors);
            } else if (words[0] === "FIELDS") {
                this.dimension = [words[1], words[2]];
            } else if (words[0] === "FD") {
                let area = new Area();
                area.district = words[3];
                area.infectionRate = words[4];
                area.population = words[5];
                if(!this.gameHistory[0]) {
                    this.gameHistory[0] = [];
                }
                if (!this.gameHistory[0][words[1]]) {
                    this.gameHistory[0][words[1]] = [];
                }
                this.gameHistory[0][words[1]][words[2]] = area;
            }
        }
    }

    display() {

    }
}

module.exports = { Reader, Area };