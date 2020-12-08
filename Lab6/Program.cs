using System;
using System.Threading;
namespace Lab6
{
    class Program
    {
        static void DrawCircle()
        {
            for (int i = 0; i < 10; i++)
            {
                Thread.Sleep(100);
            }
        }

        static void DrawSquare()
        {
            for (int i = 0; i < 10; i++)
            {
                Thread.Sleep(100);
            }
        }
        
        static void Main(string[] args)
        {
            Console.Write("\nInside main\n");
            string typeOfShape = "circle";
            int i = 0;
            for (i = 0; i < 5; i++)
            {
                switch (typeOfShape)
                {
                    case "square":
                        DrawSquare();
                        break;
                    case "circle":
                        DrawCircle();
                        break;
                }
            }
        }
    }
}