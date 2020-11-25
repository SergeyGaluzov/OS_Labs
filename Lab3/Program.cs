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
            public List<uint> pr { private set;  get; }

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
                    var index = priorityList.FindLastIndex(process => process.Priority == p.Priority);
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
            public ProcessScheduler(uint n)
            {
                priorityList = new List<Process>();
                pr = new List<uint>();
                for (uint i = 0; i < n; i++)
                {
                    Process p = new Process(i+ 1,
                        (uint) new Random().Next(1, 8),
                        (uint) new Random().Next(1, 5));
                    AddToPriorityList(p);
                }
            }
            
            /*public void Start()
            {
                const int intensivity = 6;
                int allTime = 0;
                uint t;
                uint time = 0;
                uint last_process_id = (uint)priorityList.Count;
                do
                {
                    if (time == priorityList[0].ExecutionTime)
                    {
                        t = priorityList[0].ExecutionTime;
                        for (var j = 1; j < priorityList.Count; j++)
                        {
                            priorityList[j].WaitTime -= t;
                        }
                        time = 0;

                        if (priorityList.FindAll(process => process.Priority == priorityList[0].Priority).Count == 1)
                        {
                            pr.Remove(priorityList[0].Priority);
                        }
                        priorityList.RemoveAt(0);
                    }
                    if (allTime % intensivity == 0)
                    {
                        AddToPriorityList(new Process(last_process_id + 1,
                            (uint) new Random().Next(1, 8),
                            (uint) new Random().Next(1, 5)));
                        last_process_id += 1;
                    }
                    time++;
                    allTime++;
                } while (priorityList.Any());
                Console.WriteLine(last_process_id);
            }*/

            public long ExecutionTimeSum => priorityList.Sum(process => process.ExecutionTime);
            public long WaitTimeSum => priorityList.Sum(process => process.WaitTime);

            public List<double> AvgExecutionTimeForPriority
            {
                get
                {
                    var timeList = new List<double>();
                    foreach (var priority in pr)
                    {
                        timeList.Add
                        (
                            Math.Round(priorityList.FindAll(process => process.Priority == priority)
                                .Average(process => process.ExecutionTime), 3)
                        );
                    }
                    return timeList;
                }
            }
            public List<double> AvgWaitTimeForPriority
            {
                get
                {
                    var timeList = new List<double>();
                    foreach (var priority in pr)
                    {
                        timeList.Add
                        (
                            Math.Round(priorityList.FindAll(process => process.Priority == priority)
                                .Average(process => process.WaitTime), 3)
                        );
                    }
                    return timeList;
                }
            }
        }

        static void Main(string[] args)
        {
            const uint numberOfProcesses = 100;
            ProcessScheduler scheduler = new ProcessScheduler(numberOfProcesses);
            
            Console.WriteLine($"\nTotal number of processes is: {numberOfProcesses}\n");
            
            foreach (var process in scheduler.priorityList)
            {
                Console.WriteLine($"Process priority: {process.Priority}\t" + 
                                  $"Process ID: {process.ProcessId}\t   " +
                              $"Process execution time: {process.ExecutionTime}\t" +
                                  $"Process wait time: {process.WaitTime}");
            }
            
            Console.WriteLine();
            for (var i = 0; i < scheduler.AvgExecutionTimeForPriority.Count; i++)
            {
                Console.WriteLine($"The average execution time for priority {i + 1} " +
                                  $"is {scheduler.AvgExecutionTimeForPriority[i]}");
            }
            
            Console.WriteLine();
            for (var i = 0; i < scheduler.AvgWaitTimeForPriority.Count; i++)
            {
                Console.WriteLine($"The average wait time for priority {i + 1} " +
                                  $"is {scheduler.AvgWaitTimeForPriority[i]}");
            }
            
            Console.WriteLine();
            Console.WriteLine($"The total execution time for all processes is: {scheduler.ExecutionTimeSum}");
            Console.WriteLine($"The total wait time for all processes is: {scheduler.WaitTimeSum}");
            
        }
    }
}