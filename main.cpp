#include <bits/stdc++.h>
using namespace std;

class Scorable {
public:
    virtual void addRuns(int runs) = 0;
    virtual void addBallFaced() = 0;
    virtual void addWicket() = 0;
    virtual void addOver() = 0;
    virtual void addRunsGiven(int runs) = 0;
};

class Player : public Scorable {
    string name;
    int runs, ballsFaced, wickets, runsGiven, overs;

public:
    Player(string name)
        : name(name), runs(0), ballsFaced(0),
          wickets(0), runsGiven(0), overs(0) {}

    string getName() const { return name; }

    void addRuns(int r) override { runs += r; }
    void addRunsGiven(int r) override { runsGiven += r; }
    void addBallFaced() override { ballsFaced++; }
    void addWicket() override { wickets++; }
    void addOver() override { overs++; }

    int getRuns() const { return runs; }
    int getRunsGiven() const { return runsGiven; }
    int getBallsFaced() const { return ballsFaced; }
    int getWickets() const { return wickets; }
    int getOvers() const { return overs; }

    double getStrikeRate() const {
        if (ballsFaced == 0) return 0.0;
        return (runs * 100.0) / ballsFaced;
    }
};

class Team {
    string name;
    vector<Player> players;
    int totalRuns, wicketsLost;

public:
    Team(string name) : name(name), totalRuns(0), wicketsLost(0) {}

    void addPlayer(const Player& p) {
        players.push_back(p);
    }

    string getName() const { return name; }
    vector<Player>& getPlayers() { return players; }

    void addRuns(int r) { totalRuns += r; }
    void addWicket() { wicketsLost++; }

    int getTotalRuns() const { return totalRuns; }
    int getWicketsLost() const { return wicketsLost; }
};

class CricketMatch {
protected:
    Team* team1;
    Team* team2;
    string stadium;
    Team* tossWinner;

public:
    CricketMatch(Team* t1, Team* t2, string stadium)
        : team1(t1), team2(t2), stadium(stadium), tossWinner(nullptr) {}

    virtual void conductToss() = 0;
    virtual void startMatch() = 0;
};

class CricketScorecard : public CricketMatch {
    Team* battingTeam;
    Team* bowlingTeam;

    int maxVal(int a, int b) { return max(a, b); }

public:
    CricketScorecard(Team* t1, Team* t2, string stadium)
        : CricketMatch(t1, t2, stadium) {}

    void conductToss() override {
        srand(time(0));
        tossWinner = (rand() % 2) ? team1 : team2;
        bool choice = rand() % 2;

        if (choice) {
            battingTeam = tossWinner;
            bowlingTeam = (tossWinner == team1) ? team2 : team1;
            cout << tossWinner->getName() << " won the toss and will bat first.\n";
        } else {
            bowlingTeam = tossWinner;
            battingTeam = (tossWinner == team1) ? team2 : team1;
            cout << tossWinner->getName() << " won the toss and will bowl first.\n";
        }
    }

    void playInnings(Team* bat, Team* bowl, bool chase = false) {
        vector<Player>& batsmen = bat->getPlayers();
        vector<Player>& bowlers = bowl->getPlayers();

        Player* striker = &batsmen[0];
        Player* nonStriker = &batsmen[1];
        int nextBatsman = 2;

        for (int over = 0; over < 6; over++) {
            Player* bowler = &bowlers[3 + over % 3];
            bowler->addOver();

            cout << "Over " << over + 1 << endl;
            for (int ball = 1; ball <= 6; ball++) {
                int runs;
                cout << "Enter runs for ball " << ball
                     << " (7=wicket,8=wide, negative=no-ball): ";
                cin >> runs;

                if (runs == 7) {
                    striker->addBallFaced();
                    bat->addWicket();
                    bowler->addWicket();
                    if (bat->getWicketsLost() >= 5 || nextBatsman >= batsmen.size()) {
                        cout << "All out!\n";
                        return;
                    }
                    striker = &batsmen[nextBatsman++];
                }
                else if (runs == 8) {
                    bat->addRuns(1);
                    bowler->addRunsGiven(1);
                    ball--;
                }
                else if (runs < 0) {
                    int actualRuns = abs(runs);
                    striker->addRuns(actualRuns - 1);
                    bat->addRuns(actualRuns);
                    bowler->addRunsGiven(actualRuns);
                    ball--;
                }
                else if (runs >= 0 && runs <= 6) {
                    striker->addRuns(runs);
                    striker->addBallFaced();
                    bat->addRuns(runs);
                    bowler->addRunsGiven(runs);

                    if (runs % 2) swap(striker, nonStriker);
                }

                cout << bat->getTotalRuns() << "/"
                     << bat->getWicketsLost() << endl;

                if (chase && bat->getTotalRuns() > bowl->getTotalRuns())
                    return;
            }
            swap(striker, nonStriker);
        }
    }

    void startMatch() override {
        playInnings(battingTeam, bowlingTeam, false);
        playInnings(bowlingTeam, battingTeam, true);
    }

    void writeScorecardToFile() {
        ofstream file("scorecard.txt");
        if (!file) return;

        file << "Stadium: " << stadium << "\n";
        file << "Toss Winner: " << tossWinner->getName() << "\n";

        file << "\n--- Innings 1 ---\n";
        for (auto& p : battingTeam->getPlayers())
            file << p.getName() << " Runs: " << p.getRuns()
                 << " Balls: " << p.getBallsFaced()
                 << " SR: " << p.getStrikeRate() << "\n";

        file << "\n--- Innings 2 ---\n";
        for (auto& p : bowlingTeam->getPlayers())
            file << p.getName() << " Runs: " << p.getRuns()
                 << " Balls: " << p.getBallsFaced()
                 << " SR: " << p.getStrikeRate() << "\n";

        if (battingTeam->getTotalRuns() == bowlingTeam->getTotalRuns())
            file << "\nMatch Tied\n";
        else
            file << "\nWinner: "
                 << (battingTeam->getTotalRuns() > bowlingTeam->getTotalRuns()
                     ? battingTeam->getName()
                     : bowlingTeam->getName());

        file.close();
        cout << "Scorecard saved to scorecard.txt\n";
    }
};

int main() {
    string name1, name2, stadium;

    cout << "Enter Team 1 name: ";
    getline(cin, name1);
    cout << "Enter Team 2 name: ";
    getline(cin, name2);

    Team team1(name1), team2(name2);

    cout << "Enter 6 players for Team 1:\n";
    for (int i = 0; i < 6; i++) {
        string p;
        getline(cin, p);
        team1.addPlayer(Player(p));
    }

    cout << "Enter 6 players for Team 2:\n";
    for (int i = 0; i < 6; i++) {
        string p;
        getline(cin, p);
        team2.addPlayer(Player(p));
    }

    cout << "Enter stadium name: ";
    getline(cin, stadium);

    CricketScorecard match(&team1, &team2, stadium);
    match.conductToss();
    match.startMatch();
    match.writeScorecardToFile();

    return 0;
}
