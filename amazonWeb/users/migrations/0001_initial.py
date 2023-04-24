# Generated by Django 4.1.5 on 2023-04-24 00:51

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        ('auth', '0012_alter_user_first_name_max_length'),
    ]

    operations = [
        migrations.CreateModel(
            name='Profile',
            fields=[
                ('user', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, primary_key=True, serialize=False, to=settings.AUTH_USER_MODEL)),
                ('addrX', models.CharField(max_length=10, null=True)),
                ('addrY', models.CharField(max_length=10, null=True)),
                ('upsID', models.CharField(max_length=10, null=True)),
            ],
        ),
    ]