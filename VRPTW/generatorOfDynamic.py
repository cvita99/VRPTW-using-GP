import os
import random
import sys

def parse_instance_file(filepath):
    with open(filepath, 'r') as file:
        lines = file.readlines()

    vehicle_info = []
    customer_data = []
    reading_customers = False

    for line in lines:
        line = line.strip()
        if not line:
            continue
        if line.startswith("CUSTOMER"):
            reading_customers = True
            continue
        if reading_customers:
            parts = line.split()
            if len(parts) == 7:
                customer_data.append([int(parts[0])] + list(map(float, parts[1:])))
        else:
            vehicle_info.append(line)

    return vehicle_info, customer_data

def write_instance_file(original_path, modif_root, vehicle_info, customers):
    relative_path = os.path.relpath(original_path, start='solomonInstances')
    new_path = os.path.join(modif_root, relative_path)
    os.makedirs(os.path.dirname(new_path), exist_ok=True)

    with open(new_path, 'w') as file:
        for line in vehicle_info:
            file.write(line + "\n")
        file.write("\n")
        file.write("CUST NO.  XCOORD.   YCOORD.   DEMAND  READY TIME  DUE DATE  SERVICE TIME  ORDER_VISIBLE_TIME\n")
        for cust in customers:
            file.write(f"{int(cust[0]):>6} {int(cust[1]):>9} {int(cust[2]):>9} {int(cust[3]):>8} "
                       f"{int(cust[4]):>11} {int(cust[5]):>9} {int(cust[6]):>13} {int(cust[7]):>18}\n")

def add_order_visible_times(customers, percent_dynamic):
    depot = customers[0]
    customer_list = customers[1:]

    n = len(customer_list)
    eligible_indices = [i for i, cust in enumerate(customer_list) if cust[4] > 1]
    num_gaussian = int(n * percent_dynamic / 100)
    selected_indices = set(random.sample(eligible_indices, min(num_gaussian, len(eligible_indices))))

    updated_customers = [depot + [0]]  # depot always gets ORDER_VISIBLE_TIME = 0

    for idx, customer in enumerate(customer_list):
        ready_time = customer[4]
        if idx in selected_indices:
            mu = ready_time / 2
            sigma = ready_time / 6
            sample = random.gauss(mu, sigma)
            order_visible_time = max(1, min(int(sample), int(ready_time)))
        else:
            order_visible_time = 0
        updated_customers.append(customer + [order_visible_time])

    return updated_customers

def process_all_instances(input_dir, output_dir, percent_dynamic):
    for root, _, files in os.walk(input_dir):
        for file in files:
            if file.endswith(".txt"):
                full_path = os.path.join(root, file)
                print(f"Processing {full_path} ...")
                vehicle_info, customers = parse_instance_file(full_path)
                customers = add_order_visible_times(customers, percent_dynamic)
                write_instance_file(full_path, output_dir, vehicle_info, customers)

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 make_dynamic.py <percent_dynamic_customers>")
        sys.exit(1)

    try:
        percent_dynamic = int(sys.argv[1])
        if not (0 <= percent_dynamic <= 100):
            raise ValueError
    except ValueError:
        print("Please provide an integer between 0 and 100 for dynamic customer percentage.")
        sys.exit(1)

    input_dir = "./solomonInstances"
    output_dir = "./solomonInstances_modif"
    process_all_instances(input_dir, output_dir, percent_dynamic)

if __name__ == "__main__":
    main()
