using System;
using System.Linq;
using System.Collections.Generic;

namespace Lab4
{
    class Program
    {
        class Process
        {
            public readonly string title;
            public uint executionTime;
            public uint criticalTime;

            public Process(string title)
            {
                this.title = title;
                executionTime = 0;
                criticalTime = 0;
            }
        }
        class Scheduler
        {
            private readonly List<Process> processes;
            private readonly List<List<uint>> matrix;

            public Scheduler(List<Process> processes, List<List<uint>> matrix)
            {
                this.processes = processes;
                this.matrix = matrix;
                InitExecutionTimes();
                CalculateCriticalTimes();
            }
            private void InitExecutionTimes()
            {
                for (var i = 0; i < matrix.Count; i++)
                {
                    processes[i].executionTime = matrix[i].FirstOrDefault(elem => elem != 0 && elem != int.MaxValue);
                }
            }
            
            
            private void CalculateCriticalTimes()
            {
                var queue = new List<Process> {processes.Last()};
                while (queue.Any())
                {
                    var currProcess = queue[0];
                    var previousProcessesIndexes = Enumerable.Range(0, processes.Count)
                        .Where(index => matrix[index][processes.IndexOf(currProcess)] != 0).ToList();
                    foreach (var i in previousProcessesIndexes)
                    {
                        if (processes[i].executionTime + currProcess.criticalTime > processes[i].criticalTime)
                        {
                            processes[i].criticalTime = processes[i].executionTime + currProcess.criticalTime;
                        }
                        if (!queue.Contains(processes[i])) queue.Add(processes[i]);
                    }
                    queue.RemoveAt(0);
                }
            }

            private List<Process> CalculateCriticalPath()
            {
                var currProcess = processes[0];
                var path = new List<Process>{currProcess};
                while (currProcess != processes.Last())
                {
                    var nextProcessesIndexes = Enumerable.Range(0, processes.Count)
                        .Where(index => matrix[processes.IndexOf(currProcess)][index] != 0).ToList();
                    foreach (var i in nextProcessesIndexes)
                    {
                        if (currProcess.criticalTime - currProcess.executionTime == processes[i].criticalTime)
                        {
                            currProcess = processes[i];
                            path.Add(currProcess);
                            break;
                        }
                    }
                }
                return path;
            }
            
            public void PrintCriticalPathInfo()
            {
                var path = CalculateCriticalPath();
                Console.Write("Critical path is:  ");
                for (var i = 0; i < path.Count; i++)
                {
                    Console.Write(i != path.Count - 1
                        ? $"{path[i].title}->"
                        : $"{path[i].title}");
                }
                Console.WriteLine($"\nCritical path\'s length is {processes[0].criticalTime}");
            }
        }
        static void Main(string[] args)
        {
            var processes = new List<Process>
            {
                new Process("START"),
                new Process("A"),
                new Process("B"),
                new Process("C"),
                new Process("D"),
                new Process("E"),
                new Process("F"),
                new Process("G"),
                new Process("H"),
                new Process("I"),
                new Process("J"),
                new Process("K"),
                new Process("END"),
            };
            
            const uint max = int.MaxValue;
            
            var matrix = new List<List<uint>>
            {
                new List<uint> {0,max,max,max,max,0,0,0,0,0,0,0,0},
                new List<uint> {0,0,0,0,0,2,0,0,0,0,0,0,0},
                new List<uint> {0,0,0,0,0,3,3,0,0,0,0,0,0},
                new List<uint> {0,0,0,0,0,0,0,11,11,0,0,0,0},
                new List<uint> {0,0,0,0,0,0,0,14,14,0,0,0,0},
                new List<uint> {0,0,0,0,0,0,0,0,0,5,0,0,0},
                new List<uint> {0,0,0,0,0,0,0,0,0,3,0,0,0},
                new List<uint> {0,0,0,0,0,0,0,0,0,0,0,2,0},
                new List<uint> {0,0,0,0,0,0,0,0,0,0,4,0,0},
                new List<uint> {0,0,0,0,0,0,0,0,0,0,0,13,0},
                new List<uint> {0,0,0,0,0,0,0,0,0,0,0,0,7},
                new List<uint> {0,0,0,0,0,0,0,0,0,0,0,0,18},
                new List<uint> {0,0,0,0,0,0,0,0,0,0,0,0,0}
            };
            
            var scheduler = new Scheduler(processes, matrix);
            scheduler.PrintCriticalPathInfo();
        }
    }
}
