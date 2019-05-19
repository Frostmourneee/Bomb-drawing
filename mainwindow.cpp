#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Hull.hpp"
#include <QMessageBox>
#include <QFont>
#include <QString>
#include <iostream>
#include <vector>
#include <math.h>
#include <ctime>
#include <QPainter>
#include <QPaintEvent>
#include <QPointF>
using namespace std;

//Класс точек
class Point
{
public:
    double X, Y;

    Point(double a, double b)
    {
        X = a;
        Y = b;
    }

    double distance(Point B)
    {
        return sqrt((this->X - B.X) * (this->X - B.X) + (this->Y - B.Y) * (this->Y - B.Y));
    }
};

//Класс серединных перпендикуляров к отрезку между двумя точками
class BoundaryLine
{
public:
    double A, B, C;

    BoundaryLine(Point a, Point b)
    {
        A = a.X - b.X;
        B = a.Y - b.Y;
        C = -(a.Y + b.Y) / 2 * B - (a.X + b.X) / 2 * A;
    }

};

//Функция образующая точку пересечения двух прямых
Point intersectionOfMedPerp(BoundaryLine l1, BoundaryLine l2)
{
    double E = 1.e-10;
    if (std::abs(l1.B) > E && std::abs(l2.B) > E && std::abs(l1.A / l1.B - l2.A / l2.B) > E)
    {
        return Point((l1.C / l1.B - l2.C / l2.B) / (l2.A / l2.B - l1.A / l1.B), -l1.A / l1.B * (l1.C / l1.B - l2.C / l2.B) / (l2.A / l2.B - l1.A / l1.B) - l1.C / l1.B);
    }
    if (std::abs(l1.B) < E && std::abs(l2.B) < E)
    {
        return Point(-l1.C / l1.A, l2.A * l1.C / (l1.A * l2.B) - l2.C / l2.B);
    }
    if (std::abs(l1.B) > E && std::abs(l2.B) < E)
    {
        return Point(-l2.C / l2.A, l1.A * l2.C / (l2.A * l1.B) - l1.C / l1.B);
    }
    return Point(0, 0);
}

//Проверка совпадения точек
bool samePoints(Point A, Point B)
{
    double E = 1.e-10;
    if (std::abs(A.X - B.X) < E && std::abs(A.Y - B.Y) < E) return true;
    return false;
}

//Проверка попадания всех точек в заданную окружность
bool allPointsInside(vector<Point> AllPoints, Point tempPoint, size_t N, double radius)
{
    for (size_t i = 0; i < N; i++)
    {
        if (AllPoints[i].distance(tempPoint) > radius) return false;
    }
    return true;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::setGeometry(100, 50, 1200, 600);
    MainWindow::setWindowTitle("Bomb over cities");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool a; //Проверка согласия пользователя на старт эксперимента
size_t N; //Количество требуемых точек
size_t H; //Количество различных точек
double xMax, yMax, maxNorm; //X-координата, Y-координата, радиус
vector<Point> Hull; //Точки из оболочки
vector<Point> AllMyPoints; //Все точки

void MainWindow::on_lineEdit_returnPressed()
{
    a = false;
    QString n = ui->lineEdit->text();

    if (n.toUInt(nullptr, 10))
    {
        long start_time = clock();
        ui->pushButton_2->setEnabled(1);
        ui->lineEdit->setEnabled(0);

        vector<pair<double, double>> AllBadPoints;
        vector<Point> AllPoints;
        N = n.toUInt(nullptr, 10);

        //Генерация точек
        srand(static_cast<uint>((time(nullptr))));
        AllPoints.push_back(Point(((rand() % ((ui->frame->width() - 30) / 2) - (ui->frame->width() - 30) / 4)), (rand() % ((ui->frame->height() - 30) / 2) - (ui->frame->height() - 30) / 4)));
        bool all = false;

        for (size_t i = 0; i < N - 1; i++) //Проверка отсутствия совпадающих точек
        {
            Point cand((rand() % ((ui->frame->width() - 30) / 2) - (ui->frame->width() - 30) / 4), (rand() % ((ui->frame->height() - 30) / 2) - (ui->frame->height() - 30) / 4));
            for (size_t j = 0; j < i + 1; j++)
            {
                all = samePoints(cand, AllPoints[j]);
                if (all == true)
                {
                    i--;
                    break;
                }
            }
            if (all == false) AllPoints.push_back(cand);
        }

        for (size_t i = 0; i < N; i++)
        {
            pair<double, double> a(AllPoints[i].X, AllPoints[i].Y);
            AllBadPoints.push_back(a);
        }

        vector<size_t> indexes = FindConvexHull(AllBadPoints);

        for (size_t i = 0; i < static_cast<size_t>(indexes.size()); i++)
        {
            Hull.push_back(AllPoints[indexes[i]]);
        }



        AllMyPoints = AllPoints;
        H = AllMyPoints.size(); //Количество различных точек
        AllPoints = Hull;
        N = Hull.size(); //Количество точек в выпуклой оболочке

        //Поиск максимально удалённых точек
        maxNorm = 0;
        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = i + 1; j < N; j++)
            {
                if (AllPoints[i].distance(AllPoints[j]) > 2 * maxNorm)
                {
                    xMax = (AllPoints[i].X + AllPoints[j].X) / 2.0;
                    yMax = (AllPoints[i].Y + AllPoints[j].Y) / 2.0;
                    maxNorm = AllPoints[i].distance(AllPoints[j]) / 2.0;
                }
            }
        }

        ui->label_6->setEnabled(1);

        if (allPointsInside(AllPoints, Point(xMax, yMax), N, maxNorm)) //Проверка простого случая
            //Когда окружность есть диаметр двух самых далеких точек
        {
            ui->label_4->setStyleSheet("color: rgb(0, 150, 0)");
            ui->label_7->setText("X = " + QString::number(xMax) + ";");
            ui->label_8->setText("Y = " + QString::number(yMax) + ";");
            ui->label_9->setText("R = " + QString::number(maxNorm));
        }

        else
        {   //Поиск описанной около некоторого треугольника окружности по всем точкам из оболочки,
            //Второй и последний случай
            BoundaryLine l1 = BoundaryLine(AllPoints[0], AllPoints[1]);
            BoundaryLine l2 = BoundaryLine(AllPoints[0], AllPoints[2]);
            Point center = intersectionOfMedPerp(l1, l2);
            double radius = 2 * maxNorm + 1;
            for (size_t i = 0; i < N-2; i++)
            {
                for (size_t j = i + 1; j < N-1; j++)
                {
                    for (size_t k = j + 1; k < N; k++)
                    {
                        BoundaryLine l1(AllPoints[i], AllPoints[j]);
                        BoundaryLine l2(AllPoints[i], AllPoints[k]);
                        if (allPointsInside(AllPoints, intersectionOfMedPerp(l1, l2), N, AllPoints[i].distance(intersectionOfMedPerp(l1, l2))) && AllPoints[i].distance(intersectionOfMedPerp(l1, l2)) < radius)
                        {
                            center = intersectionOfMedPerp(l1, l2);
                            radius = AllPoints[i].distance(intersectionOfMedPerp(l1, l2));
                        }
                    }
                }
            }
            xMax = center.X;
            yMax = center.Y;
            maxNorm = radius;
            ui->label_5->setStyleSheet("color: rgb(0, 150, 0)");
            ui->label_7->setText("X = " + QString::number(xMax) + ";");
            ui->label_8->setText("Y = " + QString::number(yMax) + ";");
            ui->label_9->setText("R = " + QString::number(maxNorm));
        }
        long end_time = clock();
        ui->label_3->setText(QString::number((end_time - start_time) / 1000.0));
    }

    else
    {
        QMessageBox::critical(this, "Ошибка ввода", "<font size = 4>Неверный формат для кол-ва точек<font>");
    }
    a = true;
    repaint();
}


void MainWindow::on_pushButton_2_clicked() //Кнопка завершения программы
{
    QMessageBox msgBox;
    msgBox.setGeometry(620, 320, 0, 0);
    msgBox.setText("Видимо, Вы хотите покинуть заведение?");
    msgBox.setFont(QFont("Times", 11));
    msgBox.setWindowTitle("Прости. Прощай. Привет!");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Yes, "Совершенно!");
    msgBox.setButtonText(QMessageBox::Cancel, "Остаюсь...");
    int ans = msgBox.exec();
    if (ans == QMessageBox::Yes) close();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter *paint = new QPainter(this);
    paint->setWindow(ui->frame->x(), ui->frame->y(), ui->frame->width(), ui->frame->height());
    paint->setRenderHint(QPainter::Antialiasing, true);
    paint->setPen(QPen(Qt::black, 5));

    if (a) //Начало рисования
    {
        QPointF center(((ui->frame->x()) * 2 + ui->frame->width()) / 2, ((ui->frame->y()) * 2 + ui->frame->height()) / 2);
        paint->drawPoint(center);
        paint->translate(center);
        QPointF p(0, 0);

        paint->setPen(QPen(Qt::darkGreen, 4));
        for (size_t i = 0; i < H; i++) //Все точки из оболочки
        {
            p.setX(AllMyPoints[i].X);
            p.setY(-AllMyPoints[i].Y);
            paint->drawPoint(p);
        }

        paint->setPen(QPen(Qt::blue, 5));
        QPointF base(Hull[0].X, -Hull[0].Y);
        paint->drawPoint(base);

        for (size_t i = 1; i < N; i++)//Отрисовка границ самой оболочки
        {
            paint->setPen(QPen(Qt::blue, 5));
            p.setX(Hull[i].X);
            p.setY(-Hull[i].Y);
            paint->drawPoint(p);
            paint->setPen(QPen(Qt::red, 1));
            paint->drawLine(base, p);
            base = p;
        }
        paint->drawLine(base, QPointF(Hull[0].X, -Hull[0].Y));

        paint->setPen(QPen(Qt::black, 1));
        QPointF circleCenter(xMax, -yMax);
        paint->drawEllipse(circleCenter, maxNorm, maxNorm);
        MainWindow::setGeometry(100, 50, 1200, 600);
    }
}
