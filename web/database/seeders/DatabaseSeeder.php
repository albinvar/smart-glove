<?php

namespace Database\Seeders;

// use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use App\Models\Appliance;
use Illuminate\Database\Seeder;

class DatabaseSeeder extends Seeder
{
    /**
     * Seed the application's database.
     */
    public function run(): void
    {
        // \App\Models\User::factory(10)->create();

        // \App\Models\User::factory()->create([
        //     'name' => 'Test User',
        //     'email' => 'test@example.com',
        // ]);

        // seed appliances
        Appliance::create(
            [
                'name' => 'Appliance 1',
                'status' => 0,
            ]
        );

        Appliance::create(
            [
                'name' => 'Appliance 2',
                'status' => 0,
            ]
        );
    }
}
