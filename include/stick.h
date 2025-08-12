
class Stick {
   public:
    int x = 0;
    int y = 0;
    bool changeX = false;
    bool changeY = false;

    int getNewX() {
        return newX;
    }

    int getNewY() {
        return newY;
    }

    void setNewX(int value, int steps = -1) {
        changeX = false;
        if (x == value) {
            newX = value;
            stepX = 0;
            stepsX = 0;
            return;
        }
        if (steps <= 0) {
            steps = value ? damper : 1;
        }
        newX = value;
        stepX = (float)(newX - x) / steps;
        stepsX = steps;
        changeX = true;
    }

    void setNewY(int value, int steps = -1) {
        changeY = false;
        if (y == value) {
            newY = value;
            stepY = 0;
            stepsY = 0;
            return;
        }
        if (steps <= 0) {
            steps = value ? damper : 1;
        }
        newY = value;
        stepY = (float)(newY - y) / steps;
        stepsY = steps;
        changeY = true;
    }

    void setDamper(int damper) {
        damper = round((float)damper / 5);
        if (damper <= 0) {
            damper = 1;
        } else if (damper > 20) {
            damper = 20;
        }
        this->damper = damper;
    }

    void loop() {
        if (changeX) {
            if (stepsX > 0) {
                stepsX--;
            }
            x = newX - stepX * stepsX;
            if (stepsX <= 0) {
                changeX = false;
            }
        }
        if (changeY) {
            if (stepsY > 0) {
                stepsY--;
            }
            y = newY - stepY * stepsY;
            if (stepsY <= 0) {
                changeY = false;
            }
        }
    }

   protected:
    int damper = 20;
    int stepsX = 0;
    float stepX = 0;
    int newX = 0;
    int stepsY = 0;
    float stepY = 0;
    int newY = 0;
};
