using System;
using System.Diagnostics;
namespace Lab5
{
    class Program
    {
        static void Main(string[] args)
        {
            Stopwatch stopWatch = new Stopwatch();
            stopWatch.Start();   
            int[,] b = new int[100,100];

            for (int j = 0; j < 100; j++)
            {
                for (int k = 0; k < 100; k++)
                {
                    b[k, j]++;
                }
            }
            
            stopWatch.Stop();
            var time1 = stopWatch.Elapsed.TotalMilliseconds;
            
            stopWatch.Restart();
            
            int[,] c = new int[100,100];
            
            for (int j = 0; j < 100; j++)
            {
                for (int k = 0; k < 100; k++)
                {
                    c[j, k]++;
                }
            }
            
            stopWatch.Stop();
            var time2 = stopWatch.Elapsed.TotalMilliseconds;
            
            Console.WriteLine("Execution time before optimization in ms:  " + time1);
            Console.WriteLine("Execution time after optimization in ms:  " + time2);
        }
    }
}