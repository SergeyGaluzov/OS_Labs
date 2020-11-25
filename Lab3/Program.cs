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
        
        private const int MaxExecutionProcessTime = 1000;

        class ProcessScheduler
        {
            public List<Process> priorityProcessList { private set;  get; }
            public List<uint> priorityList { private set;  get; }

            private void RecalculateProcessesWaitTimeFromIndex(int index)
            {
                for (var i = index; i < priorityProcessList.Count; i++)
                {
                    priorityProcessList[i].WaitTime =
                        i == 0 ? 0 : priorityProcessList[i - 1].WaitTime + priorityProcessList[i - 1].ExecutionTime;
                }
            }

            private void AddToPriorityProcessList(Process p)
            {
                if (priorityProcessList.Count == 0)
                {
                    priorityList.Add(p.Priority);
                    priorityProcessList.Add(p);
                }
                else
                {
                    var index = priorityProcessList.FindLastIndex(process => process.Priority == p.Priority);
                    if (index == -1)
                    {
                        priorityList.Add(p.Priority);
                        priorityList.Sort();
                        if (p.Priority == priorityList.Last())
                        {
                            index = priorityProcessList.Count;
                        }
                        else
                        {
                            index = priorityProcessList.FindIndex(process =>
                                process.Priority == priorityList.First(priority => priority > p.Priority));
                        }
                    }
                    else
                    {
                        index += 1;
                    }
                    priorityProcessList.Insert(index, p);
                    RecalculateProcessesWaitTimeFromIndex(index);
                }
            }
            public ProcessScheduler(int numberOfProcesses, int lowestPriority)
            {
                priorityProcessList = new List<Process>();
                priorityList = new List<uint>();
                for (uint i = 0; i < numberOfProcesses; i++)
                {
                    Process p = new Process(i+ 1,
                        (uint) new Random().Next(1, lowestPriority + 1),
                        (uint) new Random().Next(1, MaxExecutionProcessTime + 1));
                    AddToPriorityProcessList(p);
                }
            }
            
            /*public void Start()
            {
                const int intensivity = 6;
                int allTime = 0;
                uint t;
                uint time = 0;
                uint last_process_id = (uint)priorityProcessList.Count;
                do
                {
                    if (time == priorityProcessList[0].ExecutionTime)
                    {
                        t = priorityProcessList[0].ExecutionTime;
                        for (var j = 1; j < priorityProcessList.Count; j++)
                        {
                            priorityProcessList[j].WaitTime -= t;
                        }
                        time = 0;

                        if (priorityProcessList.FindAll(process => process.Priority == priorityProcessList[0].Priority).Count == 1)
                        {
                            priorityList.Remove(priorityProcessList[0].Priority);
                        }
                        priorityProcessList.RemoveAt(0);
                    }
                    if (allTime % intensivity == 0)
                    {
                        AddTopriorityProcessList(new Process(last_process_id + 1,
                            (uint) new Random().Next(1, 8),
                            (uint) new Random().Next(1, 5)));
                        last_process_id += 1;
                    }
                    time++;
                    allTime++;
                } while (priorityProcessList.Any());
                Console.WriteLine(last_process_id);
            }*/

            public long ExecutionTimeSum => priorityProcessList.Sum(process => process.ExecutionTime);
            public long WaitTimeSum => priorityProcessList.Sum(process => process.WaitTime);

            public List<double> AvgExecutionTimeForPriority
            {
                get
                {
                    var timeList = new List<double>();
                    foreach (var priority in priorityList)
                    {
                        timeList.Add
                        (
                            Math.Round(priorityProcessList.FindAll(process => process.Priority == priority)
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
                    foreach (var priority in priorityList)
                    {
                        timeList.Add
                        (
                            Math.Round(priorityProcessList.FindAll(process => process.Priority == priority)
                                .Average(process => process.WaitTime), 3)
                        );
                    }
                    return timeList;
                }
            }
        }

        static void Main(string[] args)
        {
            Console.Write("Enter the number of processes: ");
            var numberOfProcesses = Convert.ToInt32(Console.ReadLine());
            Console.Write("The hightest priority is 1. Enter the lowest priority: ");
            var lowestPriority = Convert.ToInt32(Console.ReadLine());
            
            ProcessScheduler scheduler = new ProcessScheduler(numberOfProcesses, lowestPriority);
            
            Console.WriteLine($"\nTotal number of processes is: {numberOfProcesses}\n");
            
            foreach (var process in scheduler.priorityProcessList)
            {
                Console.WriteLine($"Process priority: {process.Priority}\t" + 
                                  $"Process ID: {process.ProcessId}\t   " +
                              $"Process execution time: {process.ExecutionTime}\t   " +
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