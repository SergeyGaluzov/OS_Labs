using System;
using System.Linq;
using System.Collections.Generic;

namespace Lab4
{
    class Program
    {
        class Task
        {
            public readonly string title;
            public uint executionTime;
            public uint criticalTime;

            public Task(string title)
            {
                this.title = title;
                executionTime = 0;
                criticalTime = 0;
            }
        }
        class Scheduler
        {
            private readonly List<Task> tasks;
            private readonly List<List<uint>> matrix;

            public Scheduler(List<Task> tasks, List<List<uint>> matrix)
            {
                this.tasks = tasks;
                this.matrix = matrix;
                InitExecutionTimes();
                CalculateCriticalTimes();
            }
            private void InitExecutionTimes()
            {
                for (var i = 0; i < matrix.Count; i++)
                {
                    tasks[i].executionTime = matrix[i].FirstOrDefault(elem => elem != 0 && elem != int.MaxValue);
                }
            }
            
            
            private void CalculateCriticalTimes()
            {
                var queue = new List<Task> {tasks.Last()};
                while (queue.Any())
                {
                    var currTask = queue[0];
                    var previousTasksIndexes = Enumerable.Range(0, tasks.Count)
                        .Where(index => matrix[index][tasks.IndexOf(currTask)] != 0).ToList();
                    foreach (var i in previousTasksIndexes)
                    {
                        if (tasks[i].executionTime + currTask.criticalTime > tasks[i].criticalTime)
                        {
                            tasks[i].criticalTime = tasks[i].executionTime + currTask.criticalTime;
                        }
                        if (!queue.Contains(tasks[i])) queue.Add(tasks[i]);
                    }
                    queue.RemoveAt(0);
                }
            }

            private List<Task> CalculateCriticalPath()
            {
                var currTask = tasks[0];
                var path = new List<Task>{currTask};
                while (currTask != tasks.Last())
                {
                    var nextTasksIndexes = Enumerable.Range(0, tasks.Count)
                        .Where(index => matrix[tasks.IndexOf(currTask)][index] != 0).ToList();
                    foreach (var i in nextTasksIndexes)
                    {
                        if (currTask.criticalTime - currTask.executionTime == tasks[i].criticalTime)
                        {
                            currTask = tasks[i];
                            path.Add(currTask);
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
                Console.WriteLine($"\nCritical path\'s length is {tasks[0].criticalTime}");
            }
        }
        static void Main(string[] args)
        {
            var tasks = new List<Task>
            {
                new Task("START"),
                new Task("A"),
                new Task("B"),
                new Task("C"),
                new Task("D"),
                new Task("E"),
                new Task("F"),
                new Task("G"),
                new Task("H"),
                new Task("I"),
                new Task("J"),
                new Task("K"),
                new Task("END"),
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
            
            var scheduler = new Scheduler(tasks, matrix);
            scheduler.PrintCriticalPathInfo();
        }
    }
}
