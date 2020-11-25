using System;
using System.Collections.Generic;
using System.Linq;
namespace Lab3
{
    class Program
    {
        class Process
        {
            public uint ProcessId { get; }
            public uint Priority { get; }
            public uint ExecutionTime { get;}
            public uint WaitTime;

            public Process(uint id, uint priority, uint executionTime)
            {
                ProcessId = id;
                Priority = priority;
                ExecutionTime = executionTime;
                WaitTime = 0;
            }
        }

        class ProcessScheduler
        {
            public List<Process> priorityList { private set;  get; }
            private List<uint> pr;

            private void RecalculateProcessesWaitTimeFromIndex(int index)
            {
                for (var i = index; i < priorityList.Count; i++)
                {
                    priorityList[i].WaitTime =
                        i == 0 ? 0 : priorityList[i - 1].WaitTime + priorityList[i - 1].ExecutionTime;
                }
            }

            private void AddToPriorityList(Process p)
            {
                if (priorityList.Count == 0)
                {
                    pr.Add(p.Priority);
                    priorityList.Add(p);
                }
                else
                {
                    var index = priorityList.FindIndex(process => process.Priority == p.Priority);
                    if (index == -1)
                    {
                        pr.Add(p.Priority);
                        pr.Sort();
                        if (p.Priority == pr.Last())
                        {
                            index = priorityList.Count;
                        }
                        else
                        {
                            index = priorityList.FindIndex(process =>
                                process.Priority == pr.First(priority => priority > p.Priority));
                        }
                    }
                    else
                    {
                        index += 1;
                    }
                    priorityList.Insert(index, p);
                    RecalculateProcessesWaitTimeFromIndex(index);
                }
            }

            public ProcessScheduler()
            {
                priorityList = new List<Process>();
            }
            
            public ProcessScheduler(uint n) : this()
            {
                pr = new List<uint>();
                for (uint i = 0; i < n; i++)
                {
                    Process p = new Process(i + 1,
                        (uint) new Random().Next(8),
                        (uint) new Random().Next(1, 100001));
                    AddToPriorityList(p);
                }
            }
        }

        static void Main(string[] args)
        {
            const uint n = 100;
            ProcessScheduler scheduler = new ProcessScheduler(n);
            foreach (var process in scheduler.priorityList)
            {
                Console.WriteLine(process.Priority + " " + process.ProcessId + " " + process.ExecutionTime + " " +
                                  process.WaitTime);
            }

            var ExecutionTimeSum = scheduler.priorityList.Sum(process => process.ExecutionTime);
            var WaitTimeSum = scheduler.priorityList.Sum(process => process.WaitTime);
            
            
            Console.WriteLine($"{(double)ExecutionTimeSum / n}");


            const int intensivity = 5000;
            int timer = 0;
            while (scheduler.priorityList.Any())
            {
                if (timer == scheduler.priorityList[0].ExecutionTime)
                {
                    
                    timer++;
                }
            }
        }
    }
}