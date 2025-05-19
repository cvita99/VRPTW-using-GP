import os
import matplotlib.pyplot as plt

def read_customers(file_path):
    customers = {}
    with open(file_path, 'r') as f:
        lines = f.readlines()
    reading = False
    for line in lines:
        if "CUSTOMER" in line.upper():
            reading = True
            continue
        if reading:
            parts = line.strip().split()
            if len(parts) >= 7:
                try:
                    customer_id = int(parts[0])
                    x = float(parts[1])
                    y = float(parts[2])
                    customers[customer_id] = (x, y)
                except ValueError:
                    continue
    return customers

def parse_solution_block(block):
    lines = block.strip().split('\n')
    routes = []
    total_distance = None
    for line in lines:
        line = line.strip()
        if line.startswith("Printing routes"):
            continue
        if "route:" in line:
            parts = line.split('route:')[1]
            nodes = [int(node.strip()) for node in parts.split(',') if node.strip() != '']
            routes.append(nodes)
        elif "Total distance:" in line:
            total_distance = float(line.split(":")[1].strip())
    return routes, total_distance

def plot_routes(customers, routes, total_distance, instance_name, output_dir):
    plt.figure(figsize=(10, 8))
    colors = plt.cm.get_cmap('tab10', len(routes))

    # Plot customers
    for cid, (x, y) in customers.items():
        plt.scatter(x, y, c='black', s=20)
        if cid == 0:
            plt.text(x, y, f'Depot ({cid})', fontsize=8, color='red')
        else:
            plt.text(x, y, str(cid), fontsize=6)

    # Plot routes
    for idx, route in enumerate(routes):
        x_coords = [customers[cid][0] for cid in route]
        y_coords = [customers[cid][1] for cid in route]
        plt.plot(x_coords, y_coords, marker='o', label=f'Vehicle {idx+1}', color=colors(idx))

    plt.title(f'{instance_name}\nTotal distance: {total_distance:.2f}')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.legend(fontsize=8)
    plt.grid(True)
    plt.tight_layout()

    os.makedirs(output_dir, exist_ok=True)
    save_path = os.path.join(output_dir, f'{instance_name}_solution.png')
    plt.savefig(save_path)
    plt.close()
    print(f"Saved: {save_path}")

def read_solutions_and_plot(solutions_file, customers_base_dir, output_dir):
    with open(solutions_file, 'r') as f:
        content = f.read()

    # Split solutions by path line (e.g., /C1/c101.txt)
    blocks = content.split('\n/')
    first_block = blocks[0]
    blocks = blocks[1:]

    if not first_block.strip().startswith('/'):
        blocks = [first_block] + blocks

    for block in blocks:
        block = block.strip()
        if not block:
            continue
        lines = block.split('\n')
        path_line = lines[0]
        solution_block = '\n'.join(lines[1:])

        instance_path = path_line.strip()
        instance_name = os.path.splitext(os.path.basename(instance_path))[0]

        customer_file_path = os.path.join(customers_base_dir, instance_path.lstrip('/'))
        if not os.path.exists(customer_file_path):
            print(f"Warning: Customer file not found: {customer_file_path}")
            continue

        customers = read_customers(customer_file_path)
        routes, total_distance = parse_solution_block(solution_block)

        if routes and total_distance is not None:
            plot_routes(customers, routes, total_distance, instance_name, output_dir)
        else:
            print(f"Warning: Could not parse routes or distance for {instance_name}")



solutions_file = 'serial1.txt'
customers_base_dir = './solomonInstances'
output_dir = 'serialImg'

read_solutions_and_plot(solutions_file, customers_base_dir, output_dir)
